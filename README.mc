PhotoLab_server runs a server on a given port that can serve one client at a time. When provided with the AUTOTEST command, it runs several DIP processes. When provided with the PRINT command, it
provides the list of DIP processes. When provided with the CLOSE command, the server closes the socket with the client and waits for a new connection. PhotoLab_client provides these commands to
PhotoLab_server.
