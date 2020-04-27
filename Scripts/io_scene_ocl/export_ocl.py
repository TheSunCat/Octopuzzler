

# <pep8 compliant>

import os
import time

import bpy
import mathutils
import bpy_extras
import bpy_extras.io_utils as io_utils
from bpy_extras.node_shader_utils import PrincipledBSDFWrapper


def name_compat(name):
    if name is None:
        return 'None'
    else:
        return name.replace(' ', '_')


def mesh_triangulate(me):
    import bmesh
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bm.to_mesh(me)
    bm.free()


def write_file(filepath, objects, scene,
               EXPORT_TRI=True,
               EXPORT_NORMALS=False,
               EXPORT_APPLY_MODIFIERS=True,
               EXPORT_GLOBAL_MATRIX=None,
               EXPORT_PATH_MODE='AUTO',
               ):

    if EXPORT_GLOBAL_MATRIX is None:
        EXPORT_GLOBAL_MATRIX = mathutils.Matrix()

    def veckey3d(v):
        return round(v[0], 6), round(v[1], 6), round(v[2], 6)

    def veckey2d(v):
        return round(v[0], 6), round(v[1], 6)

    def findVertexGroupName(face, vWeightMap):
        """
        Searches the vertexDict to see what groups is assigned to a given face.
        We use a frequency system in order to sort out the name because a given vetex can
        belong to two or more groups at the same time. To find the right name for the face
        we list all the possible vertex group names with their frequency and then sort by
        frequency in descend order. The top element is the one shared by the highest number
        of vertices is the face's group
        """
        weightDict = {}
        for vert_index in face.vertices:
            vWeights = vWeightMap[vert_index]
            for vGroupName, weight in vWeights:
                weightDict[vGroupName] = weightDict.get(vGroupName, 0.0) + weight

        if weightDict:
            return max((weight, vGroupName) for vGroupName, weight in weightDict.items())[1]
        else:
            return '(null)'

    print('OCL Export path: %r' % filepath)

    time1 = time.time()

    file = open(filepath, "w", encoding="utf8", newline="\n")
    fw = file.write

    # Write Header
    fw('# Outrospection Collision File: %r\n' % (os.path.basename(bpy.data.filepath)))

    # Initialize totals, these are updated each object
    totverts = totno = 1

    face_vert_index = 1

    globalNormals = {}
    copy_set = set()

    # Get all meshes
    for ob_main in objects:

        # ignore duplicate children
        if ob_main.parent and ob_main.parent.instance_type in {'VERTS', 'FACES'}:
            # XXX
            print(ob_main.name, 'is an instance - ignoring')
            continue

        obs = []
        if ob_main.instance_type != 'NONE':
            # XXX
            print('creating instance_list on', ob_main.name)
            ob_main.instance_list_create(scene)

            obs = [(dob.object, dob.matrix) for dob in ob_main.instance_list]

            # XXX debug print
            print(ob_main.name, 'has', len(obs), 'instance children')
        else:
            obs = [(ob_main, ob_main.matrix_world)]

        for ob, ob_mat in obs:
            try:
                depsgraph = bpy.context.evaluated_depsgraph_get()
                me = ob.evaluated_get(depsgraph).to_mesh()
			
                # me = ob.to_mesh(scene, EXPORT_APPLY_MODIFIERS, 'PREVIEW', calc_tessface=False)
            except RuntimeError:
                me = None

            if me is None:
                continue

            me.transform(EXPORT_GLOBAL_MATRIX @ ob_mat)

            if EXPORT_TRI:
                # _must_ do this first since it re-allocs arrays
                mesh_triangulate(me)

            me_verts = me.vertices[:]

            # Make our own list so it can be sorted to reduce context switching
            face_index_pairs = [(face, index) for index, face in enumerate(me.polygons)]

            if not (len(face_index_pairs) + len(me.vertices)):  # Make sure there is something to write
                # clean up
                continue  # don't bother with this mesh.

            if EXPORT_NORMALS and face_index_pairs:
                me.calc_normals()

            # NORMAL, Smooth/Non smoothed.
            if EXPORT_NORMALS:
                for f, f_index in face_index_pairs:
                    if f.use_smooth:
                        for v_idx in f.vertices:
                            v = me_verts[v_idx]
                            noKey = veckey3d(v.normal)
                            if noKey not in globalNormals:
                                globalNormals[noKey] = totno
                                totno += 1
                                fw('n %.6f %.6f %.6f\n' % noKey)
                    else:
                        # Hard, 1 normal from the face.
                        noKey = veckey3d(f.normal)
                        if noKey not in globalNormals:
                            globalNormals[noKey] = totno
                            totno += 1
                            fw('n %.6f %.6f %.6f\n' % noKey)


            for f, f_index in face_index_pairs:
                f_v = [(vi, me_verts[v_idx]) for vi, v_idx in enumerate(f.vertices)]

                if EXPORT_NORMALS and not f_smooth:
                    no = globalNormals[veckey3d(f.normal)]

                for vi, v in f_v:
                    fw("%.6f %.6f %.6f" % me_verts[(v.index)].co[:]) # + totverts?
                    if EXPORT_NORMALS:
                        fw('/')
                        fw('/%d' % no)

                fw('\n')


            # Make the indices global rather then per mesh
            totverts += len(me_verts)

        if ob_main.instance_type != 'NONE':
            ob_main.instance_list_clear()
            
    file.close()

    # copy all collected files.
    bpy_extras.io_utils.path_reference_copy(copy_set)

    print("OCL Export time: %.2f" % (time.time() - time1))


def _write(context, filepath,
              EXPORT_TRI,  # ok
              EXPORT_NORMALS,  # not yet
              EXPORT_APPLY_MODIFIERS,  # ok
              EXPORT_SEL_ONLY,  # ok
              EXPORT_GLOBAL_MATRIX,
              EXPORT_PATH_MODE,
              ):  # Not used

    base_name, ext = os.path.splitext(filepath)
    context_name = [base_name, '', '', ext]  # Base name, scene name, frame number, extension

    scene = context.scene

    # Exit edit mode before exporting, so current object states are exported properly.
    if bpy.ops.object.mode_set.poll():
        bpy.ops.object.mode_set(mode='OBJECT')

    orig_frame = scene.frame_current

    scene_frames = [orig_frame]  # Dont export an animation.

    # Loop through all frames in the scene and export.
    for frame in scene_frames:

        scene.frame_set(frame, subframe=0.0)
        if EXPORT_SEL_ONLY:
            objects = context.selected_objects
        else:
            objects = scene.objects

        full_path = ''.join(context_name)

        # erm... bit of a problem here, this can overwrite files when exporting frames. not too bad.
        # EXPORT THE FILE.
        write_file(full_path, objects, scene,
                   EXPORT_TRI,
                   EXPORT_NORMALS,
                   EXPORT_APPLY_MODIFIERS,
                   EXPORT_GLOBAL_MATRIX,
                   EXPORT_PATH_MODE,
                   )

    scene.frame_set(orig_frame, subframe=0.0)

    # Restore old active scene.
#   orig_scene.makeCurrent()
#   Window.WaitCursor(0)


"""
Currently the exporter lacks these features:
* multiple scene export (only active scene is written)
* particles
"""


def save(operator, context, filepath="",
         use_triangles=True,
         use_normals=False,
         use_mesh_modifiers=True,
         use_selection=True,
         global_matrix=None,
         path_mode='AUTO'
         ):

    _write(context, filepath,
           EXPORT_TRI=use_triangles,
           EXPORT_NORMALS=use_normals,
           EXPORT_APPLY_MODIFIERS=use_mesh_modifiers,
           EXPORT_SEL_ONLY=use_selection,
           EXPORT_GLOBAL_MATRIX=global_matrix,
           EXPORT_PATH_MODE=path_mode,
           )

    return {'FINISHED'}
