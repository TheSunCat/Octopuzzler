# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

# <pep8-80 compliant>

bl_info = {
    "name": "Export Outrospection Model Format (.omd)",
    "author": "TheSunCat",
    "blender": (2, 80, 0),
    "location": "File > Export > Outrospection Model (.omd)",
    "description": "Export OMD",
    "warning": "",
    "tracker_url": "",
    "category": "Import-Export"}

if "bpy" in locals():
    import imp
    if "export_omd" in locals():
        imp.reload(export_omd)


import bpy
from bpy.props import (BoolProperty,
                       FloatProperty,
                       StringProperty,
                       EnumProperty,
                       )
from bpy_extras.io_utils import (ImportHelper,
                                 ExportHelper,
                                 path_reference_mode,
                                 axis_conversion,
                                 )


class ExportOMD(bpy.types.Operator, ExportHelper):
    """Save an Outrospection OMD File"""

    bl_idname = "export_scene.outrospection_omd"
    bl_label = 'Export OMD'
    bl_options = {'PRESET'}

    filename_ext = ".omd"
    
    filter_glob : StringProperty(
            default="*.omd",
            options={'HIDDEN'},
            )

    # context group
    use_selection : BoolProperty(
            name="Selection Only",
            description="Export selected objects only",
            default=False,
            )

    # object group
    use_mesh_modifiers : BoolProperty(
            name="Apply Modifiers",
            description="Apply modifiers (preview resolution)",
            default=True,
            )
    use_normals : BoolProperty(
            name="Include Normals",
            description="",
            default=True,
            )
    use_colors : BoolProperty(
            name="Include Vertex Colors",
            description="Write out the active vertex colors",
            default=False,
            )
    use_triangles : BoolProperty(
            name="Triangulate Faces",
            description="Convert all faces to triangles",
            default=True,
            )
    group_by_material : BoolProperty(
            name="Material Groups",
            description="",
            default=False,
            )

    axis_forward : EnumProperty(
            name="Forward",
            items=(('X', "X Forward", ""),
                   ('Y', "Y Forward", ""),
                   ('Z', "Z Forward", ""),
                   ('-X', "-X Forward", ""),
                   ('-Y', "-Y Forward", ""),
                   ('-Z', "-Z Forward", ""),
                   ),
            default='-Z',
            )
    axis_up : EnumProperty(
            name="Up",
            items=(('X', "X Up", ""),
                   ('Y', "Y Up", ""),
                   ('Z', "Z Up", ""),
                   ('-X', "-X Up", ""),
                   ('-Y', "-Y Up", ""),
                   ('-Z', "-Z Up", ""),
                   ),
            default='Y',
            )
    global_scale : FloatProperty(
            name="Scale",
            min=0.01, max=1000.0,
            default=1.0,
            )

    path_mode = path_reference_mode

    check_extension = True

    def execute(self, context):
        from . import export_omd

        from mathutils import Matrix
        keywords = self.as_keywords(ignore=("axis_forward",
                                            "axis_up",
                                            "global_scale",
                                            "check_existing",
                                            "filter_glob",
                                            ))

        global_matrix = (Matrix.Scale(self.global_scale, 4) @
                         axis_conversion(to_forward=self.axis_forward,
                                         to_up=self.axis_up,
                                         ).to_4x4())

        keywords["global_matrix"] = global_matrix
        return export_omd.save(self, context, **keywords)


def menu_func_export(self, context):
    self.layout.operator(ExportOMD.bl_idname, text="Outrospection Model (.omd)")


def register():
    from bpy.utils import register_class
    register_class(ExportOMD)

    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    from bpy.utils import unregister_class
    unregister_class(ExportOMD)

    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()
