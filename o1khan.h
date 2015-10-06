#ifndef O1KHAN_H
#define O1KHAN_H
#include "o1.h"


/*
 * Try following the documentation in your OAuth library and configuring it to
 * use these three authentication endpoints:

https://www.khanacademy.org/api/auth2/request_token
https://www.khanacademy.org/api/auth2/authorize
https://www.khanacademy.org/api/auth2/access_token

Consumer Key:	uCj7JaPgnqVPaPDD
Consumer Secret:	FcwDL3xeY826yGUY

 * */
class O1Khan: public O1 {
    Q_OBJECT

public:
    explicit O1Khan(QObject *parent = 0): O1(parent) {
        setRequestTokenUrl(QUrl("https://www.khanacademy.org/api/auth2/request_token"));
        setAuthorizeUrl(QUrl("https://www.khanacademy.org/api/auth2/authorize"));
        setAccessTokenUrl(QUrl("https://www.khanacademy.org/api/auth2/access_token"));
    }
};


#endif // O1KHAN_H
