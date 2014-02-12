/*
 *----------------------------------------------------------------------------
 *
 * msktutil.h
 *
 * (C) 2004-2006 Dan Perry (dperry@pppl.gov)
 * (C) 2006 Brian Elliott Finley (finley@anl.gov)
 * (C) 2009-2010 Doug Engert (deengert@anl.gov)
 * (C) 2010 James Y Knight (foom@fuhm.net)
 *
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *-----------------------------------------------------------------------------
 */

#ifndef __msktutil_h__
#define __msktutil_h__


#include "config.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <ldap.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>


#ifdef HAVE_COM_ERR_H
# ifdef COM_ERR_NEEDS_EXTERN_C
  extern "C" {
# endif
#include <com_err.h>
# ifdef COM_ERR_NEEDS_EXTERN_C
 }
# endif
#endif
#include <krb5.h>
#ifdef HAVE_SASL_H
#include <sasl.h>
#else
#include <sasl/sasl.h>
#endif

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <memory>

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "msktutil"
#endif
#define PASSWORD_LEN                    63
#define MAX_HOSTNAME_LEN                255
#define MAX_TRIES                       10
#define MAX_SAM_ACCOUNT_LEN             20
#define MAX_DEF_MACH_PASS_LEN		14
#define MAX_DOMAIN_CONTROLLERS		20


#ifndef TMP_DIR
#define TMP_DIR                         "/tmp"
#endif


/* In case it's not in krb5.h */
#ifndef MAX_KEYTAB_NAME_LEN
#define  MAX_KEYTAB_NAME_LEN            1100
#endif

/* From SAM.H */
#define UF_WORKSTATION_TRUST_ACCOUNT    0x00001000
#define UF_NORMAL_ACCOUNT               0x00000200
#define UF_DONT_EXPIRE_PASSWORD         0x00010000
#define UF_TRUSTED_FOR_DELEGATION       0x00080000
#define UF_USE_DES_KEY_ONLY             0x00200000
#define UF_NO_AUTH_DATA_REQUIRED        0x02000000

/* for msDs-supportedEncryptionTypes  bit defines */
#define MS_KERB_ENCTYPE_DES_CBC_CRC             0x01
#define MS_KERB_ENCTYPE_DES_CBC_MD5             0x02
#define MS_KERB_ENCTYPE_RC4_HMAC_MD5            0x04

// Define these if the system supports them, otherwise define to 0.
#if HAVE_DECL_ENCTYPE_AES128_CTS_HMAC_SHA1_96
#define MS_KERB_ENCTYPE_AES128_CTC_HMAC_SHA1_96 0x08
#else
#define MS_KERB_ENCTYPE_AES128_CTC_HMAC_SHA1_96 0
#endif

#if HAVE_DECL_ENCTYPE_AES128_CTS_HMAC_SHA1_96
#define MS_KERB_ENCTYPE_AES256_CTS_HMAC_SHA1_96 0x10
#else
#define MS_KERB_ENCTYPE_AES256_CTS_HMAC_SHA1_96 0
#endif

/* Some KVNO Constansts */
#define KVNO_FAILURE                    -1
#define KVNO_WIN_2000                   0

/* LDAP Binding Attempts */
#define ATTEMPT_SASL_PARAMS_TLS         0
#define ATTEMPT_SASL_NO_PARAMS_TLS      1
#define ATTEMPT_SASL_NO_TLS             2

/* Ways we can authenticate */
enum auth_types {
    AUTH_NONE = 0,
    AUTH_FROM_SAM_KEYTAB,
    AUTH_FROM_HOSTNAME_KEYTAB,
    AUTH_FROM_PASSWORD,
    AUTH_FROM_USER_CREDS,
    AUTH_FROM_SUPPLIED_PASSWORD,
    AUTH_FROM_SUPPLIED_EXPIRED_PASSWORD,
    AUTH_FROM_EXPLICIT_KEYTAB,
};

class LDAPConnection;

enum msktutil_val {
    VALUE_OFF = 0,
    VALUE_ON = 1,
    VALUE_IGNORE = 2
};

extern int g_verbose;

enum msktutil_mode {
    MODE_NONE = 0,
    MODE_CREATE,
    MODE_UPDATE,
    MODE_AUTO_UPDATE,
    MODE_FLUSH,
    MODE_PRECREATE
};

struct msktutil_flags {
    std::string keytab_file;
    std::string keytab_writename;
    std::string keytab_readname;
    std::string keytab_auth_princ;
    std::string ldap_ou;
    std::string hostname;
    std::map<std::string, std::string> attributes;
    std::string server;
    std::string realm_name;
    std::string lower_realm_name;
    std::string base_dn;
    std::string samAccountName;
    std::string samAccountName_nodollar;
    std::string password;
    std::string userPrincipalName;
    std::string unicodePwd;
    std::string old_account_password;
    std::string site;
    std::auto_ptr<LDAPConnection> ldap;

    std::string ad_computerDn;
    std::string ad_dnsHostName;
    std::string ad_userPrincipal;
    std::vector<std::string> ad_principals;

    bool set_userPrincipalName;
    bool no_reverse_lookups;
    bool server_behind_nat;
    bool set_samba_secret;

    msktutil_val dont_expire_password;
    msktutil_val no_pac;
    msktutil_val delegate;
    unsigned int ad_userAccountControl; /* value AD has now */
    int ad_enctypes;    /* if msDs-supportedEncryptionTypes in AD */
    unsigned int ad_supportedEncryptionTypes; /* value AD has now */
    int enctypes;       /* if --enctypes parameter was set */
    unsigned int supportedEncryptionTypes;

    int auth_type;
    bool user_creds_only;
    bool use_service_account;
    bool allow_weak_crypto;
    bool password_expired;
    msktutil_flags();
    ~msktutil_flags();
};

struct msktutil_exec {
    msktutil_mode mode;
    std::vector<std::string> add_principals;
    std::vector<std::string> remove_principals;
    msktutil_flags *flags;

    msktutil_exec();
    ~msktutil_exec();
};

struct msktutil_dcdata {
    char srvname[NS_MAXDNAME];
    unsigned int priority;
    unsigned int weight;
    unsigned int port; 
};

/* Prototypes */
extern std::string create_default_machine_password(const std::string &samaccountname);
extern void ldap_cleanup(msktutil_flags *);
extern void init_password(msktutil_flags *);
extern std::string get_default_hostname();
extern void get_default_keytab(msktutil_flags *);
extern void get_default_ou(msktutil_flags *);
extern std::auto_ptr<LDAPConnection> ldap_connect(const std::string &server,
                                                  bool no_reverse_lookups = false,
                                                  int try_tls=ATTEMPT_SASL_PARAMS_TLS);
extern void ldap_get_base_dn(msktutil_flags *);
extern std::string complete_hostname(const std::string &);
extern std::string get_short_hostname(msktutil_flags *);
extern int flush_keytab(msktutil_flags *);
extern void update_keytab(msktutil_flags *);
extern void add_principal_keytab(const std::string &, krb5_kvno kvno, msktutil_flags *);
extern int ldap_flush_principals(msktutil_flags *);
extern int set_password(msktutil_flags *, int time = 0);
extern krb5_kvno ldap_get_kvno(msktutil_flags *);
extern std::string ldap_get_pwdLastSet(msktutil_flags *);
extern std::vector<std::string> ldap_list_principals(msktutil_flags *);
extern int ldap_add_principal(const std::string &, msktutil_flags *);
int ldap_remove_principal(const std::string &principal, msktutil_flags *flags);
extern std::string get_dc_host(const std::string &realm_name, const std::string &site_name,
                               const bool);
extern std::string get_user_principal();
extern std::string get_host_os();
extern void ldap_check_account(msktutil_flags *);
extern void create_fake_krb5_conf(msktutil_flags *);
extern void remove_fake_krb5_conf();
extern void remove_ccache();
int find_working_creds(msktutil_flags *flags);
bool get_creds(msktutil_flags *flags);
int generate_new_password(msktutil_flags *flags);

/* Verbose messages */
#define VERBOSE(text...) if (g_verbose) { fprintf(stdout, " -- %s: ", __FUNCTION__); fprintf(stdout, ## text); fprintf(stdout, "\n"); }


#define VERBOSEldap(text...) if (g_verbose > 1) { fprintf(stderr, " ###### %s: ", __FUNCTION__); fprintf(stderr, ## text); fprintf(stderr, "\n"); }
#endif

// printf into a C++ string.
std::string sform(const char* format, ...);

class Exception : public std::exception
{
  protected:
    std::string m_message;

    // Prohibit assignment
    Exception& operator=(const Exception&);

  public:
    // Constructors

    // Default construction with no message uses "Exception"
    Exception() : m_message("Exception") { }
    explicit Exception(char const * simple_string) : m_message(simple_string) {}
    explicit Exception(const std::string &str) : m_message(str) {}
    Exception(const Exception& src) : exception(), m_message(src.m_message)  {}

    virtual ~Exception() throw() {};
    char const * what() const throw() { return m_message.c_str(); }
};

class KRB5Exception : public Exception
{
  protected:
    krb5_error_code m_err;
  public:
    explicit KRB5Exception(const std::string &func, krb5_error_code err) :
        Exception(sform("Error: %s failed (%s)", func.c_str(), error_message(err)))
    { m_err = err; }
    krb5_error_code err() const throw() { return m_err; }

};

class LDAPException : public Exception
{
  public:
    explicit LDAPException(const std::string &func, int err) :
        Exception(sform("Error: %s failed (%s)", func.c_str(), ldap_err2string(err)))
    {}
};

#ifdef __GNUC__
#define ATTRUNUSED __attribute__((unused))
#else
#define ATTRUNUSED
#endif


#include "krb5wrap.h"



class LDAPConnection {
public: //fixme
    LDAP *m_ldap;

public:
    LDAPConnection(const std::string &server);

    void set_option(int option, const void *invalue);
    void get_option(int option, void *outvalue);
    void start_tls(LDAPControl **serverctrls=NULL, LDAPControl **clientctrls=NULL);

    void search(LDAPMessage **mesg_p,
                const std::string &base_dn, int scope, const std::string &filter, char *attrs[],
                int attrsonly=0, LDAPControl **serverctrls=NULL, LDAPControl **clientctrls=NULL,
                struct timeval *timeout=NULL, int sizelimit=-1);

    std::string get_one_val(LDAPMessage *mesg, char *name);
    std::vector<std::string> get_all_vals(LDAPMessage *mesg, char *name);
    ~LDAPConnection();
};
