#ifndef IRODS_FILESYSTEM_CONFIG_HPP
#define IRODS_FILESYSTEM_CONFIG_HPP

#include <irods/rcConnect.h>

#ifdef RODS_SERVER
using conn = rsComm_t;
#else
using conn = rcComm_t;
#endif // RODS_SERVER

#endif // IRODS_FILESYSTEM_CONFIG_HPP
