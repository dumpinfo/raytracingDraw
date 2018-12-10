#include <stdio.h>

#include <canvas.h>
#include <render.h>
#include <obj_loader.h>

#define CANVAS_W 400
#define CANVAS_H 400

// Boost by rendering in parallel
#define THREADS_NUM 4

#define BACKGROUND_COLOR rgb(255, 255, 255)

#define MAX_OBJECTS_NUMBER 10000
#define MAX_LIGHT_SOURCES_NUMBER 5

int
main(void) {   
    // Allocating scene
    Scene * scene = new_scene(MAX_OBJECTS_NUMBER,
                              MAX_LIGHT_SOURCES_NUMBER,
                              BACKGROUND_COLOR);
    
    // Allocating new sphere
    Float radius = 100;
    Point3d center = point3d(0, 0, 0);
    Color sphere_color = rgb(250, 30, 30);
    Material sphere_material = material(1, 5, 5, 10, 0, 10);
    Object3d * sphere = new_sphere(center,
                                   radius,
                                   sphere_color,
                                   sphere_material);
    
    // Adding sphere to the scene
    add_object(scene,
               sphere);
    
    // Allocating new triangle
    Object3d * triangle = new_triangle(point3d(-700, -700, -130), // vertex 1
                                       point3d( 700, -700, -130), // vertex 2
                                       point3d(   0,  400, -130), // vertex 3
                                       rgb(100, 255, 30),         // color
                                       material(1, 6, 0, 2, 0, 0) // surface params
                                       );
    
    // Adding triangle to the scene
    add_object(scene,
               triangle);
    
    // Loading 3D model of cow from *.obj file
    // defining transformations and parameters of 3D model
    // TODO: must be refactored...
    SceneFaceHandlerParams load_params =
    new_scene_face_handler_params(scene,
                                  // scale:
                                  40,
                                  // move dx, dy, dz:
                                  -150, -100, 30,
                                  // rotate around axises x, y, z:
                                  0, 0, 0,
                                  // color
                                  rgb(200, 200, 50),
                                  // surface params
                                  material(2, 3, 0, 0, 0, 0)
                                  );
    
    load_obj("./demo/models/cow.obj",
             // default handler which adding polygons of 3D model to scene:
             scene_face_handler,
             &load_params);
    
    // This function is requried (bulding k-d tree of entire scene)
    prepare_scene(scene);
    
    printf("\nNumber of polygons: %i\n", scene->last_object_index + 1);
    
    // Allocating new light source
    Color light_source_color = rgb(255, 255, 255);
    Point3d light_source_location = point3d(-300, 300, 300);
    LightSource3d * light_source = new_light_source(light_source_location,
                                                    light_source_color);
    // Adding light source to the scene
    add_light_source(scene,
                     light_source);
    
    // Adding fog
    Float density = 0.002;
    set_exponential_fog(scene, density);
    
    // Allocating camera
    // TODO: It's a pity, but quaternions are not implemented yet :(
    Point3d camera_location = point3d(0, 500, 0);
    Float focus = 320;
    Float x_angle = -1.57;
    Float y_angle = 0;
    Float z_angle = 3.14;
    Camera * camera = new_camera(camera_location,
                                 x_angle,
                                 y_angle,
                                 z_angle,
                                 focus);
    
    // Rotate camera if needed
    // rotate_camera(camera, d_x_angle, d_y_angle, d_z_angle);
    
    // Move camera if needed
    // move_camera(camera, vector3df(d_x, d_y, d_z));
    
    // Alocate new canvas, to render scene on it
    Canvas * canvas = new_canvas(CANVAS_W,
                                 CANVAS_H);
    
    render_scene(scene,
                 camera,
                 canvas,
                 THREADS_NUM);
    
    // Saving rendered image in PNG format
    write_png("example.png",
              canvas);
    
    Canvas * grayscaled_canvas = grayscale_canvas(canvas,
                                                  THREADS_NUM);
    write_png("gray_example.png",
              grayscaled_canvas);
    
    Canvas * edges_canvas = detect_edges_canvas(canvas,
                                                THREADS_NUM);
    write_png("edges_example.png",
              edges_canvas);
    
    release_canvas(canvas);
    release_canvas(grayscaled_canvas);
    release_canvas(edges_canvas);
    release_scene(scene);
    release_camera(camera);
    
    return 0;
}