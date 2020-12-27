#ifndef SOLVER_KP01_GENERIC_H
#define SOLVER_KP01_GENERIC_H

extern "C"
{
   int
   solver_kp01_generic(int format, const char* config, char* output);

   int
   info_kp01_generic(int format, const char* key, char* output);

} // extern "C"

#endif // SOLVER_KP01_GENERIC_H
