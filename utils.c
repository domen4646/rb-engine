#include "common.h"

unsigned int get_window_width() { return 800; }
unsigned int get_window_height() { return 600; }

void __serr() {
    printf("SDL Error\n");
    printf("%s\n", SDL_GetError());
    _exit(1);
}

void _serr(const char* fname) {
    printf("SDL Error (%s failed)\n", fname);
    printf("%s\n", SDL_GetError());
    _exit(1);
}

bool util_c_str_endswith(char* str, char* end) {
    if (str == NULL || end == NULL) {
        false;
    }
    int len1 = strlen(str);
    int len2 = strlen(end);
    if (len1 < len2) {
        return false;
    }
    return !strcmp(str + len1 - len2, end);
}

bool cvector_find_and_remove(cvector_vector_type(VALUE) vec, VALUE val) {
    for (int i=0; i<cvector_size(vec); i++) {
        if (vec[i] == val) {
            cvector_erase(vec, i);
            return true;
        }
    }
    return false;
}

void cvector_find_and_remove_all(cvector_vector_type(VALUE) vec, VALUE val) {
    while (cvector_find_and_remove(vec, val)) {};
}

bool cvector_find_and_remove_viewport(cvector_vector_type(VIEWPORT*) vec, VIEWPORT* val) {
    for (int i=0; i<cvector_size(vec); i++) {
        if (vec[i] == val) {
            cvector_erase(vec, i);
            return true;
        }
    }
    return false;
}

void cvector_find_and_remove_all_viewports(cvector_vector_type(VIEWPORT*) vec, VIEWPORT* val) {
    while (cvector_find_and_remove_viewport(vec, val)) {};
}

int limit(int val, int min, int max) {
    return (val < min) ? min : ((val > max) ? max : val);
}



// Collision
void sprite_support(const void* obj, const ccd_vec3_t* dir, ccd_vec3_t* vec) {
    (SPRITE*) s = (SPRITE*) obj;
    ccd_vec3_t direction;
    ccd_vec3_t pos;
    ccd_quat_t qinv;

    ccdVec3Set(&pos, s->destination.x, s->destination.y, 0);

    // apply rotation on direction vector
    ccdVec3Copy(&direction, dir);
    ccdQuatInvert2(&qinv, &s->_internal_quaternion);
    ccdQuatRotVec(&direction, &qinv);

    // compute support point in specified direction
    ccdVec3Set(vec, ccdSign(ccdVec3X(&direction)) * s->destination.w * CCD_REAL(0.5),
                  ccdSign(ccdVec3Y(&direction)) * s->destination.h * CCD_REAL(0.5),
                  ccdSign(0);
                  //ccdSign(ccdVec3Z(&direction)) * box->z * CCD_REAL(0.5));

    // transform support point according to position and rotation of object
    ccdQuatRotVec(vec, &s->_internal_quaternion);
    ccdVec3Add(vec, &pos);
}




#ifdef __linux__

#include <dirent.h>

int util_rb_filelist(RB_FILELIST_CALLBACK callback) {
    DIR* dir = opendir(".");
    if (dir == NULL) {
        return -1;
    }
    struct dirent* d;

    while ((d = readdir(dir)) != NULL) {
        if (!util_c_str_endswith(d->d_name, ".rb"))
            continue;
        callback(d->d_name);
    }

    closedir(dir);
    return 0;
}

#endif
