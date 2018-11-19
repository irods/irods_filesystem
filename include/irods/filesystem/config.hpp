#ifndef IRODS_FILESYSTEM_CONFIG_HPP
#define IRODS_FILESYSTEM_CONFIG_HPP

#ifdef RODS_SERVER
    #define rxConn                  rsComm_t

    #define rxOpenCollection        rsOpenCollection
    #define rxReadCollection        rsReadCollection
    #define rxCloseCollection       rsCloseCollection
    #define rxObjStat               rsObjStat
    #define rxModColl               rsModColl
    #define rxCollCreate            rsCollCreate
    #define rxRmColl                rsRmColl 
    #define rxDataObjCopy           rsDataObjCopy
    #define rxDataObjRename         rsDataObjRename
    #define rxDataObjUnlink         rsDataObjUnlink
    #define rxModAccessControl      rsModAccessControl
#else
    #define rxConn                  rcComm_t

    #define rxOpenCollection        rcOpenCollection
    #define rxReadCollection        rcReadCollection
    #define rxCloseCollection       rcCloseCollection
    #define rxObjStat               rcObjStat
    #define rxModColl               rcModColl
    #define rxCollCreate            rcCollCreate
    #define rxRmColl                rcRmColl
    #define rxDataObjCopy           rcDataObjCopy
    #define rxDataObjRename         rcDataObjRename
    #define rxDataObjUnlink         rcDataObjUnlink
    #define rxModAccessControl      rcModAccessControl
#endif // RODS_SERVER

#endif // IRODS_FILESYSTEM_CONFIG_HPP
