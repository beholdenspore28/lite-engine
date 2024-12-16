"""
    LMOD File exporter. 
    Originally designed for use in lite-engine.
    
    This is a simple Blender 4.3 script for exporting a selected
    3d model using the .lmod file format.

    .lmod specification is as follows:
        
        # this is a comment

        #mesh: is a keyword. all keywords should end with a colon.

        mesh: # <- this is a mesh

            # to add a list of vertex_positions to the mesh, use the vertex_position keyword.

            vertex_positions:
                6.9420 8.675309 3.14159 # all vector members are separated by one or
                ...                     # more space but must remain on one line.

            # to define a list of vertex normals, use the vertex_normal keyword.

            vertex_normals:
                0.577350 0.577350 0.577350 # normal vectors should be written as unit vectors
                ...                        # (eg always having a length of 1)
"""

import bpy
from bpy import context

selectedObjects = context.selected_objects

filepath = "/home/benis/repos/lite-engine/res/models/untitled.lmod"

objects = selectedObjects

with open(filepath, 'w') as f:
    f.write("# LMOD file\n")
    for obj in objects:
        mesh = obj.data
       
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.mesh.quads_convert_to_tris(quad_method='FIXED', ngon_method='BEAUTY')
        bpy.ops.object.mode_set(mode='OBJECT')

        f.write("mesh: %s\n" % mesh.name)

        f.write("vertex_positions:\n")
        for v in mesh.vertices:
            f.write("%.4f\t%.4f\t%.4f\n" % v.co[:])

        f.write("vertex_normals:\n")
        for v in mesh.vertices:
            f.write("%.4f\t%.4f\t%.4f\n" % v.normal[:])

        f.write("vertex_indices:\n")
        for p in mesh.polygons:
            for v in reversed(p.vertices):
                f.write(f"{v} ")
        f.write("\n")