/// \file
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#include "LogCommandParser.h"
#include "RakNetworkFactory.h"
#include "RakServerInterface.h"
#include "RakClientInterface.h"
#include "RakServer.h"
#include "RakClient.h"
#include "RakPeerInterface.h"
#include "RakPeer.h"

RakClientInterface* RakNetworkFactory::GetRakClientInterface( void )
{
	return new RakClient;
}
RakServerInterface* RakNetworkFactory::GetRakServerInterface( void )
{
	return new RakServer;
}
RakPeerInterface* RakNetworkFactory::GetRakPeerInterface( void )
{
	return new RakPeer;
}

void RakNetworkFactory::DestroyRakClientInterface( RakClientInterface* i )
{
	delete ( RakClient* ) i;
}
void RakNetworkFactory::DestroyRakServerInterface( RakServerInterface* i )
{
	delete ( RakServer* ) i;
}
void RakNetworkFactory::DestroyRakPeerInterface( RakPeerInterface* i )
{
	delete ( RakPeer* ) i;
}
