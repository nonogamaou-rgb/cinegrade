#include <mod/amlmod.h>
#include <GLES2/gl2.h>
#include <dlfcn.h>
#include <string>

MYMOD(net.local.cinegrade, CineGrade, 1.0, me)

// ===== PENGATURAN =====
#define BRIGHTNESS "0.03"
#define CONTRAST   "1.10"
#define SATURATION "1.10"
#define WARM       "vec3(1.06,1.0,0.92)"
#define SHADOW     "0.18"
#define BLOOM      "0.30"
// ======================

static const char* GRADE =
"\nvoid main(){ orig_main();"
" mediump vec3 c=_fc.rgb*" WARM ";"
" float l=dot(c,vec3(0.299,0.587,0.114));"
" c=mix(vec3(l),c," SATURATION ");"
" c=(c-0.5)*" CONTRAST "+0.5+" BRIGHTNESS ";"
" c*=1.0-(1.0-smoothstep(0.0,0.5,l))*" SHADOW ";"
" c+=c*max(l-0.78,0.0)*" BLOOM "*4.0;"
" gl_FragColor=vec4(clamp(c,0.0,1.0),_fc.a); }\n";

static void replaceAll(std::string& s, const std::string& a, const std::string& b) {
    size_t p = 0;
    while ((p = s.find(a, p)) != std::string::npos) { s.replace(p, a.size(), b); p += b.size(); }
}

static void (*orig)(GLuint, GLsizei, const GLchar* const*, const GLint*);

static void hook(GLuint sh, GLsizei n, const GLchar* const* src, const GLint* len) {
    std::string code;
    for (int i = 0; i < n; i++)
        code += (len && len[i] > 0) ? std::string(src[i], len[i]) : std::string(src[i]);

    if (code.find("gl_FragColor") != std::string::npos &&
        code.find("void main") != std::string::npos &&
        code.find("#version") == std::string::npos &&
        code.find("gl_FragData") == std::string::npos) {
        replaceAll(code, "gl_FragColor", "_fc");
        size_t p = code.find("void main");
        code.replace(p, 9, "mediump vec4 _fc;\nvoid orig_main");
        code += GRADE;
    }
    const char* out = code.c_str();
    orig(sh, 1, &out, nullptr);
}

extern "C" void OnModPreLoad() {
    void* gl = dlopen("libGLESv2.so", RTLD_NOW);
    void* fn = gl ? dlsym(gl, "glShaderSource") : nullptr;
    if (fn) aml->Hook(fn, (void*)hook, (void**)&orig);
}
