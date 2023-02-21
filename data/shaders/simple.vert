#version 330 core

in vec3 vtx_position;
in vec3 vtx_color;
out vec3 var_color;
in mat4 M_inv;
in mat4 transform;
in vec3 n;
in vec3 v;
in vec3 l;
in float s;

// uniform mat4 M;
uniform mat4 mat_cam;

// Attention aux translations des vecteurs.

/*
n = normal
v = view direction
l = light direction
dCol = diffuse color
sCol = specular color
s = shininess
*/
vec3 blinn(vec3 n, vec3 v, vec3 l, vec3 dCol, vec3 sCol, float s)
{

    
    // h est le half vecteur. Bissecteur entre v et l.
    vec3 h = (l + v)/length(l + v);

    float cos_teta = max(dot(n, l), 0);
    float cos_beta = pow(max(dot(n, h), 0), s);

    float diff = max(dot(n, l), 0);
    float spec = max(dot(-l, h), 0);

    return diff * dCol + spec * sCol;

}

void main()
{
    // Exprimer les trois vecteurs dans le même repère.
    // Transformer la position du sommet dans le repère caméra. 
    M_inv = mat_cam.inverse();
    transform = M_inv.transpose();

    n = (M_inv * vec4(n, 0)).xyz;
    v = (M_inv * vec4(v, 0)).xyz;
    l = (M_inv * vec4(l, 0)).xyz;

    dCol = vtx_color;
    sCol = vtx_color;
    s = 1.;

    gl_Position = mat_cam * vec4(vtx_position, 1.);
    // vtx_color;

    var_color = blinn(n, l, v, dCol, sCol, s);
}
