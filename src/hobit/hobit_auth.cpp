/***************************************************************************
 *   Copyright (C) 2004 by Walter Roth                                     *
 *   walter@amd2600-roth                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "hobit_auth.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>

#include <qapplication.h>


HoBIT_Auth::HoBIT_Auth(API *hbci, QWidget* parent):Auth(hbci) {
  _secrets = 0;
  _secrets=GWEN_DB_Group_new("secrets");
  _parent = parent;
}



HoBIT_Auth::~HoBIT_Auth(){
  GWEN_DB_Group_free(_secrets);
  qDebug ("Destructor HoBIT_Auth");
}



Error HoBIT_Auth::getSecret(Pointer<User> user,
                             const string &token,
                             string &secret,
                             bool createSecret){
  const char *p;
  cerr << "HoBIT_Auth::getSecret called for token: "<<token.c_str()<< endl;
  p=GWEN_DB_GetCharValue(_secrets, token.c_str(), 0, 0);
  if (!p) {
    DBG_INFO(0, "Secret \"%s\" not found, asking", token.c_str());
    return Auth::getSecret(user, token, secret, createSecret);
  }

  secret=p;
  qDebug("Returning secret: %s", p);
  return Error();
}



Error HoBIT_Auth::readPinList(const string &fname){
  struct stat st;
  struct passwd *pw;

  GWEN_DB_ClearGroup(_secrets, 0);
  if (stat(fname.c_str(), &st)) {
    fprintf(stderr,
            "Error: stat(%s): %s\n",
            fname.c_str(), strerror(errno));
    return Error("AqMoneyAuth::readPinList",
                 ERROR_LEVEL_NORMAL,
                 HBCI_ERROR_CODE_INVALID,
                 ERROR_ADVISE_DONTKNOW,
                 "error stat'ing file");
  }

  if ((st.st_mode & S_IRWXG) ||
      (st.st_mode & S_IRWXO)) {
    fprintf(stderr,
            "=======================================\n"
                 " Very, very serious warning !!!\n"
                 " Your PIN file is world/group readable.\n"
                 " This really is very, very bad !\n"
                 " Evil people are able to read your PINs.\n"
                 " Therefore I abort now.\n");
    return Error("AqMoneyAuth::readPinList",
                 ERROR_LEVEL_NORMAL,
                 HBCI_ERROR_CODE_INVALID,
                 ERROR_ADVISE_DONTKNOW,
                 "bad file mode");
  }

  pw=getpwuid(st.st_uid);
  if (!pw) {
    fprintf(stderr,
            "Could not get shell info for owner of PIN file\n");
    return Error("AqMoneyAuth::readPinList",
                 ERROR_LEVEL_NORMAL,
                 HBCI_ERROR_CODE_INVALID,
                 ERROR_ADVISE_DONTKNOW,
                 "bad owner");
  }

  if (strcasecmp(pw->pw_shell, "/bin/false")!=0) {
    fprintf(stderr,
            "=========================================================\n"
                 " Serious warning !!\n"
                 " You should not use the PIN file as normal user !\n"
                 " This way all processes started by you can read the\n"
                 " PIN file.\n"
                 " I recommend:\n"
                 "  - creating a special user and group for AqMoney\n"
                 "  - setting his shell to \"/bin/false\"\n"
                 "  - setting the AqMoney executable to SETUID\n"
                 "  - add yourself to the newly created group\n"
                 "  - CHOWN the PIN file to the new user\n"
                 "  - CHMOD the PIN file so that only the owner can read it\n"
                 " This way even you as normal user can not read the\n"
                 " PIN file, and  neither can any process started by you\n");
  }

  if (GWEN_DB_ReadFile(_secrets, fname.c_str(),
                       GWEN_DB_FLAGS_DEFAULT |
                       GWEN_PATH_FLAGS_CREATE_GROUP)) {
    return Error("AqMoneyAuth::readPinList",
                 ERROR_LEVEL_NORMAL,
                 HBCI_ERROR_CODE_INVALID,
                 ERROR_ADVISE_DONTKNOW,
                 "error reading file");
  }
  return Error();
}




void HoBIT_Auth::setSecretStatus(Pointer<User> user,
                                  const string &token,
                                  string &secret,
                                  HBCI_AUTH_STATUS st){
  const char *p;
  cerr<< "HoBIT_Auth::setSecretStatus called with status:" << st <<endl;
  p=GWEN_DB_GetCharValue(_secrets, token.c_str(), 0, 0);
  if (!p) {
    DBG_INFO(0, "Secret \"%s\" not found", token.c_str());
  }
  else {
    if (st==HBCI_AUTH_STATUS_BAD) {
      DBG_WARN(0, "Secret \"%s\" is bad, please check your PIN file !!!",
               token.c_str());
      GWEN_DB_DeleteVar(_secrets, token.c_str());
    }
    else if (st==HBCI_AUTH_STATUS_OK) {
      DBG_INFO(0, "Secret \"%s\" is ok.", token.c_str());
    }
  }
}



HBCI_AUTH_STATUS HoBIT_Auth::getSecretStatus(Pointer<User> user,
                                              const string &token,
                                              string &secret){
  const char *p;

  p=GWEN_DB_GetCharValue(_secrets, token.c_str(), 0, 0);
  if (!p)
    return HBCI_AUTH_STATUS_UNKNOWN;
  return HBCI_AUTH_STATUS_OK;
}

