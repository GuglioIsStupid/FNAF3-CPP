#include "gl2d.hpp"
#include <cstdio>

namespace Core {
namespace Rendering {
namespace GL2D {

static GLuint s_program = 0;
static GLint s_programSDF = -1;

static GLuint s_vao = 0;
static GLuint s_vbo = 0;

static GLint s_locUseTex = -1;
static GLint s_locTex = -1;

static GLuint compile(GLenum type, const char* src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok = 0; glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024]; GLsizei n=0; glGetShaderInfoLog(sh, sizeof log, &n, log);
        std::fprintf(stderr, "GL2D shader compile error: %.*s\n", (int)n, log);
        glDeleteShader(sh); return 0;
    }
    return sh;
}

static GLuint link(GLuint vs, GLuint fs) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint ok = 0; glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024]; GLsizei n=0; glGetProgramInfoLog(prog, sizeof log, &n, log);
        std::fprintf(stderr, "GL2D program link error: %.*s\n", (int)n, log);
        glDeleteProgram(prog); return 0;
    }
    return prog;
}

bool init() {
    if (s_program) return true;
    const char* vsSrc = R"GLSL(
        #version 330 core
        layout(location=0) in vec2 aPos;
        layout(location=1) in vec2 aUV;
        layout(location=2) in vec4 aColor;
        out vec2 vUV;
        out vec4 vColor;
        void main() {
            vUV = aUV;
            vColor = aColor;
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )GLSL";

    const char* fsSrc = R"GLSL(
        #version 330 core
        in vec2 vUV;
        in vec4 vColor;
        uniform bool uUseTex;
        uniform sampler2D uTex;
        out vec4 oColor;
        void main() {
            vec4 col = vColor;
            if (uUseTex) {
                col *= texture(uTex, vUV);
            }
            oColor = col;
        }
    )GLSL";

    const char* fsSDF = R"GLSL(
        #version 330 core
        in vec2 vUV;
        in vec4 vColor;

        uniform sampler2D uTex;

        out vec4 oColor;

        void main() {
            float sdf = texture(uTex, vUV).r;

            float width = fwidth(sdf);
            float alpha = smoothstep(0.5 - width,
                                    0.5 + width,
                                    sdf);

            oColor = vec4(vColor.rgb, vColor.a * alpha);
        }
    )GLSL";

    GLuint vs = compile(GL_VERTEX_SHADER, vsSrc);
    if (!vs) return false;
    GLuint fs = compile(GL_FRAGMENT_SHADER, fsSrc);
    if (!fs) { glDeleteShader(vs); return false; }
    s_program = link(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLuint fs_sdf = compile(GL_FRAGMENT_SHADER, fsSDF);
    s_programSDF = link(vs, fs_sdf);
    glDeleteShader(fs_sdf);
    if (!s_program) return false;

    s_locUseTex = glGetUniformLocation(s_program, "uUseTex");
    s_locTex = glGetUniformLocation(s_program, "uTex");

    glGenVertexArrays(1, &s_vao);
    glBindVertexArray(s_vao);
    glGenBuffers(1, &s_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBufferData(GL_ARRAY_BUFFER, 4096, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0); // aPos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(1); // aUV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(2); // aColor
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(s_programSDF);
    glUniform1i(glGetUniformLocation(s_programSDF,"uTex"),0);
    glUseProgram(0);

    return true;
}

void shutdown() {
    if (s_vbo) { glDeleteBuffers(1, &s_vbo); s_vbo = 0; }
    if (s_vao) { glDeleteVertexArrays(1, &s_vao); s_vao = 0; }
    if (s_program) { glDeleteProgram(s_program); s_program = 0; }
}

void draw(GLenum mode, const Vertex* verts, size_t count, GLuint texture, bool sdf) {
    if (!s_program || !s_vao || !s_vbo || !verts || count == 0) return;
    glUseProgram(sdf && s_programSDF ? s_programSDF : s_program);
    glBindVertexArray(s_vao);

    GLsizeiptr bytes = (GLsizeiptr)(count * sizeof(Vertex));
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    GLint curSize = 0; glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &curSize);
    if (bytes > curSize) {
        glBufferData(GL_ARRAY_BUFFER, bytes, verts, GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, verts);
    }

    if (texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        if (s_locTex >= 0) glUniform1i(s_locTex, 0);
        if (s_locUseTex >= 0) glUniform1i(s_locUseTex, 1);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        if (s_locUseTex >= 0) glUniform1i(s_locUseTex, 0);
    }

    glDrawArrays(mode, 0, (GLsizei)count);

    glBindVertexArray(0);
    glUseProgram(0);
}

void drawTriangles(const Vertex* verts, size_t count, GLuint texture, bool sdf) {
    draw(GL_TRIANGLES, verts, count, texture, sdf);
}

void drawTriangles(const std::vector<Vertex>& verts, GLuint texture, bool sdf) {
    draw(GL_TRIANGLES, verts.data(), verts.size(), texture, sdf);
}

} // namespace GL2D
} // namespace Rendering
} // namespace Core
