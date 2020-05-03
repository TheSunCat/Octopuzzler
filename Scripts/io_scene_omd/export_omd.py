

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


def write_mtl(scene, filepath, fw, path_mode, copy_set, mtl_dict):
    source_dir = os.path.dirname(bpy.data.filepath)
    dest_dir = os.path.dirname(filepath)

    fw('\n# Outrospection Material Data\n')
    fw('# Material Count: %i' % len(mtl_dict))

    mtl_dict_values = list(mtl_dict.values())
    mtl_dict_values.sort(key=lambda m: m[0])

    # Write material/image combinations we have used.
    # Using mtl_dict.values() directly gives un-predictable order.
    for mtl_mat_name, mat in mtl_dict_values:
        # Get the Blender data for the material and the image.
        # Having an image named None will make a bug, dont do it :)

        fw('\nmn %s\n' % mtl_mat_name)  # Define a new material: matname_imgname

        mat_wrap = PrincipledBSDFWrapper(mat) if mat else None

        if mat_wrap:
            use_mirror = mat_wrap.metallic != 0.0
            use_transparency = mat_wrap.alpha != 1.0

            # XXX Totally empirical conversion, trying to adapt it
            #     (from 1.0 - 0.0 Principled BSDF range to 0.0 - 900.0 OBJ specular exponent range)...
            spec = (1.0 - mat_wrap.roughness) * 30
            spec *= spec
            fw('s %.6f\n' % spec)

            # Ambient
            if use_mirror:
                fw('Ka %.6f %.6f %.6f\n' % (mat_wrap.metallic, mat_wrap.metallic, mat_wrap.metallic))
            else:
                fw('Ka %.6f %.6f %.6f\n' % (1.0, 1.0, 1.0))
            fw('Kd %.6f %.6f %.6f\n' % mat_wrap.base_color[:3])  # Diffuse
            # XXX TODO Find a way to handle tint and diffuse color, in a consistent way with import...
            fw('Ks %.6f %.6f %.6f\n' % (mat_wrap.specular, mat_wrap.specular, mat_wrap.specular))  # Specular

            #### And now, the image textures...
            image_map = {
                    "Md": "base_color_texture",
                    "Ma": None,  # ambient...
                    "Ms": "specular_texture",
                    }

            for key, mat_wrap_key in sorted(image_map.items()):
                if mat_wrap_key is None:
                    continue
                    
                tex_wrap = getattr(mat_wrap, mat_wrap_key, None)
                if tex_wrap is None:
                    continue
                    
                image = tex_wrap.image
                if image is None:
                    continue

                # TODO copy the files from their path to ../Textures
                filepath = io_utils.path_reference(image.filepath, source_dir, dest_dir,
                                                   path_mode, "", copy_set, image.library)

                fw('%s %s\n' % (key, repr(filepath).replace('\\', '/')[repr(filepath).replace('\\', '/').rindex('/') + 1:-1]))

        else:
            # Write a dummy material here?
            fw('Ns 500\n')
            fw('Ka 0.8 0.8 0.8\n')
            fw('Kd 0.8 0.8 0.8\n')
            fw('Ks 0.8 0.8 0.8\n')

def write_file(filepath, objects, scene,
               EXPORT_TRI=True,
               EXPORT_NORMALS=False,
               EXPORT_COLORS=False,
               EXPORT_UV=True,
               EXPORT_MTL=True,
               EXPORT_APPLY_MODIFIERS=True,
               EXPORT_GROUP_BY_MAT=False,
               EXPORT_KEEP_VERT_ORDER=False,
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

    print('OMD Export path: %r' % filepath)

    time1 = time.time()

    file = open(filepath, "w", encoding="utf8", newline="\n")
    fw = file.write

    # Write Header
    fw('# Outrospection OMD File: %r\n' % (os.path.basename(bpy.data.filepath)))

    # Tell the obj file what material file to use.
    if EXPORT_MTL:
        mtlfilepath = os.path.splitext(filepath)[0] + ".mtl"

    # Initialize totals, these are updated each object
    totverts = totuvco = totno = totcolors = 1

    face_vert_index = 1

    globalNormals = {}

    # A Dict of Materials
    # (material.name, image.name):matname_imagename # matname_imagename has gaps removed.
    mtl_dict = {}
    # Used to reduce the usage of matname_texname materials, which can become annoying in case of
    # repeated exports/imports, yet keeping unique mat names per keys!
    # mtl_name: (material.name, image.name)
    mtl_rev_dict = {}

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
			
            except RuntimeError:
                me = None

            if me is None:
                continue

            me.transform(EXPORT_GLOBAL_MATRIX @ ob_mat)

            if EXPORT_TRI:
                # _must_ do this first since it re-allocs arrays
                mesh_triangulate(me)

            if EXPORT_UV:
                faceuv = len(me.uv_layers) > 0
                if faceuv:
                    uv_layer = me.uv_layers.active.data[:]
            else:
                faceuv = False

            if EXPORT_COLORS and me.vertex_colors.active is not None:
                facecolor = True
                color_layer = me.vertex_colors.active.data[:]
            else:
                facecolor = False

            me_verts = me.vertices[:]

            # Make our own list so it can be sorted to reduce context switching
            face_index_pairs = [(face, index) for index, face in enumerate(me.polygons)]

            if not (len(face_index_pairs) + len(me.vertices)):  # Make sure there is something to write
                # clean up
                bpy.data.meshes.remove(me)

                continue  # don't bother with this mesh.

            if EXPORT_NORMALS and face_index_pairs:
                me.calc_normals()

            smooth_groups, smooth_groups_tot = (), 0

            materials = me.materials[:]
            material_names = [m.name if m else None for m in materials]

            # avoid bad index errors
            if not materials:
                materials = [None]
                material_names = [name_compat(None)]

            # Sort by Material, then images
            # so we don't over context switch in the obj file.
            if EXPORT_KEEP_VERT_ORDER:
                pass
            else:
                if len(materials) > 1:
                    if smooth_groups:
                        sort_func = lambda a: (a[0].material_index,
                                               smooth_groups[a[1]] if a[0].use_smooth else False)
                    else:
                        sort_func = lambda a: (a[0].material_index,
                                               a[0].use_smooth)
                else:
                    # no materials
                    if smooth_groups:
                        sort_func = lambda a: smooth_groups[a[1] if a[0].use_smooth else False]
                    else:
                        sort_func = lambda a: a[0].use_smooth
                face_index_pairs.sort(key=sort_func)

                del sort_func

            # Set the default mat to no material and no image.
            contextMat = 0, 0  # Can never be this, so we will label a new material the first chance we get.
            contextSmooth = None  # Will either be true or false,  set bad to force initialization switch.

            # Write the object name
            name1 = ob.name
            name2 = ob.data.name
            if name1 == name2:
                obnamestring = name_compat(name1)
            else:
                obnamestring = '%s_%s' % (name_compat(name1), name_compat(name2))
                
            fw('o %s\n' % obnamestring)

            # Vert
            for v in me_verts:
                fw('v %.6f %.6f %.6f\n' % v.co[:])

            # UV
            if faceuv:
                # in case removing some of these don't get defined.
                uv = uvkey = uv_dict = f_index = uv_index = uv_ls = uv_k = None

                uv_face_mapping = [None] * len(face_index_pairs)

                uv_dict = {}  # could use a set() here
                for f, f_index in face_index_pairs:
                    uv_ls = uv_face_mapping[f_index] = []
                    for uv_index, l_index in enumerate(f.loop_indices):
                        uv = uv_layer[l_index].uv

                        uvkey = veckey2d(uv)
                        try:
                            uv_k = uv_dict[uvkey]
                        except:
                            uv_k = uv_dict[uvkey] = len(uv_dict)
                            fw('t %.6f %.6f\n' % uv[:])
                        uv_ls.append(uv_k)

                uv_unique_count = len(uv_dict)

                del uv, uvkey, uv_dict, f_index, uv_index, uv_ls, uv_k
                # Only need uv_unique_count and uv_face_mapping

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

            # COLOR
            if facecolor:
                # in case removing some of these don't get defined.
                color = colorkey = color_dict = f_index = color_index = color_ls = color_k = None

                color_face_mapping = [None] * len(face_index_pairs)

                color_dict = {}  # could use a set() here
                for f, f_index in face_index_pairs:
                    color_ls = color_face_mapping[f_index] = []
                    for color_index, l_index in enumerate(f.loop_indices):
                        color = color_layer[l_index].color

                        colorkey = veckey3d(color)
                        try:
                            color_k = color_dict[colorkey]
                        except:
                            color_k = color_dict[colorkey] = len(color_dict)
                            fw('c %.6f %.6f %.6f\n' % color[:])
                        color_ls.append(color_k)

                color_unique_count = len(color_dict)

                del color, colorkey, color_dict, f_index, color_index, color_ls, color_k
                # Only need color_unique_count and color_face_mapping

            for f, f_index in face_index_pairs:
                f_smooth = f.use_smooth
                if f_smooth and smooth_groups:
                    f_smooth = smooth_groups[f_index]
                f_mat = min(f.material_index, len(materials) - 1)

                # MAKE KEY
                key = material_names[f_mat], None  # No image, use None instead.

                # CHECK FOR CONTEXT SWITCH
                if key == contextMat:
                    pass  # Context already switched, don't do anything
                else:
                    if key[0] is None and key[1] is None:
                        # Write a null material, since we know the context has changed.
                        if EXPORT_GROUP_BY_MAT:
                            # can be mat_image or (null)
                            fw("g %s_%s\n" % (name_compat(ob.name), name_compat(ob.data.name)))  # can be mat_image or (null)
                        if EXPORT_MTL:
                            fw("mu (null)\n")  # mat, image

                    else:
                        mat_data = mtl_dict.get(key)
                        if not mat_data:
                            # First add to global dict so we can export to mtl
                            # Then write mtl

                            # Make a new names from the mat and image name,
                            # converting any spaces to underscores with name_compat.

                            # If none image dont bother adding it to the name
                            # Try to avoid as much as possible adding texname (or other things)
                            # to the mtl name (see [#32102])...
                            mtl_name = "%s" % name_compat(key[0])
                            if mtl_rev_dict.get(mtl_name, None) not in {key, None}:
                                if key[1] is None:
                                    tmp_ext = "_NONE"
                                else:
                                    tmp_ext = "_%s" % name_compat(key[1])
                                i = 0
                                while mtl_rev_dict.get(mtl_name + tmp_ext, None) not in {key, None}:
                                    i += 1
                                    tmp_ext = "_%3d" % i
                                mtl_name += tmp_ext
                            mat_data = mtl_dict[key] = mtl_name, materials[f_mat]
                            mtl_rev_dict[mtl_name] = key

                        if EXPORT_GROUP_BY_MAT:
                            fw("g %s_%s_%s\n" % (name_compat(ob.name), name_compat(ob.data.name), mat_data[0]))  # can be mat_image or (null)
                        if EXPORT_MTL:
                            fw("mu %s\n" % mat_data[0])  # can be mat_image or (null)

                contextMat = key
                if f_smooth != contextSmooth:
                    if f_smooth:  # on now off
                        if smooth_groups:
                            f_smooth = smooth_groups[f_index]
                    contextSmooth = f_smooth

                f_v = [(vi, me_verts[v_idx]) for vi, v_idx in enumerate(f.vertices)]

                if EXPORT_NORMALS and not f_smooth:
                    no = globalNormals[veckey3d(f.normal)]

                fw('f')

                for vi, v in f_v:
                    fw(" %d" % (v.index + totverts))
                    if faceuv:
                        fw('/%d' % (totuvco + uv_face_mapping[f_index][vi]))
                    elif EXPORT_NORMALS or facecolor:
                        fw('/')
                    if EXPORT_NORMALS:
                        if f_smooth:
                            fw('/%d' % globalNormals[veckey3d(v.normal)])
                        else:
                            fw('/%d' % no)
                    elif facecolor:
                        fw('/')
                    if facecolor:
                        fw('/%d' % (totcolors + color_face_mapping[f_index][vi]))

                fw('\n')


            # Make the indices global rather then per mesh
            totverts += len(me_verts)
            if faceuv:
                totuvco += uv_unique_count
            if facecolor:
                totcolors += color_unique_count

            # clean up
            #bpy.data.meshes.remove(me)

        if ob_main.instance_type != 'NONE':
            ob_main.instance_list_clear()

    # Now we have all our materials, save them
    if EXPORT_MTL:
        write_mtl(scene, filepath, fw, EXPORT_PATH_MODE, copy_set, mtl_dict)

    file.close()

    # copy all collected files.
    bpy_extras.io_utils.path_reference_copy(copy_set)

    print("OBJ Export time: %.2f" % (time.time() - time1))


def _write(context, filepath,
              EXPORT_TRI,  # ok
              EXPORT_NORMALS,  # not yet
              EXPORT_COLORS,
              EXPORT_UV,  # ok
              EXPORT_MTL,
              EXPORT_APPLY_MODIFIERS,  # ok
              EXPORT_GROUP_BY_MAT,
              EXPORT_KEEP_VERT_ORDER,
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
                   EXPORT_COLORS,
                   EXPORT_UV,
                   EXPORT_MTL,
                   EXPORT_APPLY_MODIFIERS,
                   EXPORT_GROUP_BY_MAT,
                   EXPORT_KEEP_VERT_ORDER,
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
         use_normals=True,
         use_colors=False,
         use_uvs=True,
         use_materials=True,
         use_mesh_modifiers=True,
         group_by_material=False,
         keep_vertex_order=False,
         use_selection=True,
         global_matrix=None,
         path_mode='AUTO'
         ):

    _write(context, filepath,
           EXPORT_TRI=use_triangles,
           EXPORT_NORMALS=use_normals,
           EXPORT_COLORS=use_colors,
           EXPORT_UV=use_uvs,
           EXPORT_MTL=use_materials,
           EXPORT_APPLY_MODIFIERS=use_mesh_modifiers,
           EXPORT_GROUP_BY_MAT=group_by_material,
           EXPORT_KEEP_VERT_ORDER=keep_vertex_order,
           EXPORT_SEL_ONLY=use_selection,
           EXPORT_GLOBAL_MATRIX=global_matrix,
           EXPORT_PATH_MODE=path_mode,
           )

    return {'FINISHED'}
