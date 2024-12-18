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
import bmesh
from bpy import context

selectedObjects = context.selected_objects
filepath = "/home/benis/repos/lite-engine/res/models/untitled.lmod"
objects = selectedObjects

with open(filepath, 'w') as f:
    f.write("# LMOD file\n")
    for obj in objects:
        mesh = obj.data
       
        bm = bmesh.new()
        bm.from_mesh(mesh)
        bmesh.ops.triangulate(bm, faces=bm.faces)
        bm.free()

        f.write("mesh: %s\n" % mesh.name)

        f.write("vertex_positions:\n")
        for v in mesh.vertices:
            f.write("%.4f\t%.4f\t%.4f\n" % (v.co.x, v.co.z, -v.co.y))

        f.write("vertex_normals:\n")
        for v in mesh.vertices:
            f.write("%.4f\t%.4f\t%.4f\n" % (v.normal.x, v.normal.z, -v.normal.y))

        f.write("vertex_indices:\n")
        for loopt in mesh.loop_triangles:
            for v in reversed(loopt.vertices):
                f.write(f"{v} ")

        f.write("\n")
