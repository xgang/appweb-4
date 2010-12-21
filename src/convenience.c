/*
    convenience.c -- High level convenience API
    This module provides simple, high-level APIs for creating servers.
    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/********************************** Includes **********************************/

#include    "appweb.h"

/************************************ Code ************************************/
/*  
    Create a web server described by a config file. 
 */
MaServer *maCreateWebServer(cchar *configFile)
{
    Mpr         *mpr;
    MaAppweb    *appweb;
    MaServer    *server;

    /*  
        Initialize and start the portable runtime services.
     */
    if ((mpr = mprCreate(0, NULL, 0)) == 0) {
        mprError("Can't create the web server runtime");
        return 0;
    }
    if (mprStart() < 0) {
        mprError("Can't start the web server runtime");
        return 0;
    }
    if ((appweb = maCreateAppweb(mpr)) == 0) {
        mprError("Can't create appweb object");
        return 0;
    }
    if ((server = maCreateServer(appweb, "default", ".", NULL, 0)) == 0) {
        mprError("Can't create the web server");
        return 0;
    }
    if (maParseConfig(server, configFile) < 0) {
        mprError("Can't parse the config file %s", configFile);
        return 0;
    }
    return server;
}


/*  
    Service requests for a web server.
 */
int maServiceWebServer(MaServer *server)
{
    if (maStartServer(server) < 0) {
        mprError("Can't start the web server");
        return MPR_ERR_CANT_CREATE;
    }
    mprServiceEvents(mprGetDispatcher(server), -1, 0);
    maStopServer(server);
    return 0;
}


/*  
    Run a web server using a config file. 
 */
int maRunWebServer(cchar *configFile)
{
    MaServer  *server;

    if ((server = maCreateWebServer(configFile)) == 0) {
        return MPR_ERR_CANT_CREATE;
    }
    return maServiceWebServer(server);
}


int maRunSimpleWebServer(cchar *ip, int port, cchar *docRoot)
{
    Mpr         *mpr;
    MaServer    *server;
    MaAppweb    *appweb;

    /*  
        Initialize and start the portable runtime services.
     */
    if ((mpr = mprCreate(0, NULL, 0)) == 0) {
        mprError("Can't create the web server runtime");
        return MPR_ERR_CANT_CREATE;
    }
    if (mprStart(mpr) < 0) {
        mprError("Can't start the web server runtime");
        return MPR_ERR_CANT_INITIALIZE;
    }
    //  TODO MOB - do we need a meta server when running via an API
    if ((appweb = maCreateAppweb(mpr)) == 0) {
        mprError("Can't create the web server http services");
        return MPR_ERR_CANT_INITIALIZE;
    }

    /*  
        Create and start the HTTP server. Give the server a name of "default" and define "." as the default serverRoot, 
        ie. the directory with the server configuration files.
     */
    server = maCreateServer(appweb, ip, ".", ip, port);
    if (server == 0) {
        mprError("Can't create the web server");
        return MPR_ERR_CANT_CREATE;
    }
    maSetHostDocumentRoot(server->defaultHost, docRoot);
    
    if (maStartServer(server) < 0) {
        mprError("Can't start the web server");
        return MPR_ERR_CANT_CREATE;
    }
    mprServiceEvents(mprGetDispatcher(mpr), -1, 0);
    maStopServer(server);
    mprStop(mpr);
    mprFree(mpr);
    return 0;
}


/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.

    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire
    a commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.TXT distributed with
    this software for full details.

    This software is open source; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version. See the GNU General Public License for more
    details at: http://www.embedthis.com/downloads/gplLicense.html

    This program is distributed WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    This GPL license does NOT permit incorporating this software into
    proprietary programs. If you are unable to comply with the GPL, you must
    acquire a commercial license to use this software. Commercial licenses
    for this software and support services are available from Embedthis
    Software at http://www.embedthis.com

    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=8 ts=8 expandtab

    @end
 */
