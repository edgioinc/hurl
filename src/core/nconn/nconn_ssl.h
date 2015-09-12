//: ----------------------------------------------------------------------------
//: Copyright (C) 2014 Verizon.  All Rights Reserved.
//: All Rights Reserved
//:
//: \file:    nconn_ssl.h
//: \details: TODO
//: \author:  Reed P. Morrison
//: \date:    02/07/2014
//:
//:   Licensed under the Apache License, Version 2.0 (the "License");
//:   you may not use this file except in compliance with the License.
//:   You may obtain a copy of the License at
//:
//:       http://www.apache.org/licenses/LICENSE-2.0
//:
//:   Unless required by applicable law or agreed to in writing, software
//:   distributed under the License is distributed on an "AS IS" BASIS,
//:   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//:   See the License for the specific language governing permissions and
//:   limitations under the License.
//:
//: ----------------------------------------------------------------------------
#ifndef _NCONN_SSL_H
#define _NCONN_SSL_H

//: ----------------------------------------------------------------------------
//: Includes
//: ----------------------------------------------------------------------------
#include "nconn_tcp.h"
#include "ndebug.h"
#include <openssl/ssl.h>

//: ----------------------------------------------------------------------------
//: Constants
//: ----------------------------------------------------------------------------

namespace ns_hlx {

//: ----------------------------------------------------------------------------
//: Fwd Decl's
//: ----------------------------------------------------------------------------

//: ----------------------------------------------------------------------------
//: Enums
//: ----------------------------------------------------------------------------


//: ----------------------------------------------------------------------------
//: \details: TODO
//: ----------------------------------------------------------------------------
class nconn_ssl: public nconn_tcp
{
public:
        // ---------------------------------------
        // Options
        // ---------------------------------------
        typedef enum ssl_opt_enum
        {
                OPT_SSL_CTX = 1000,

                // Settings
                OPT_SSL_CIPHER_STR = 1001,
                OPT_SSL_OPTIONS = 1002,

                // Verify options
                OPT_SSL_VERIFY = 1100,
                OPT_SSL_VERIFY_ALLOW_SELF_SIGNED = 1101,

                // CA options
                OPT_SSL_CA_FILE = 1201,
                OPT_SSL_CA_PATH = 1202,

                OPT_SSL_SENTINEL = 1999

        } ssl_opt_t;

        nconn_ssl(bool a_verbose,
                  bool a_color,
                  int64_t a_max_reqs_per_conn = -1,
                  bool a_save_response_in_reqlet = false,
                  bool a_collect_stats = false,
                  bool a_connect_only = false,
                  type_t a_type = TYPE_CLIENT):
                          nconn_tcp(a_verbose,
                        	    a_color,
                        	    a_max_reqs_per_conn,
                        	    a_save_response_in_reqlet,
                        	    a_collect_stats,
                        	    a_connect_only,
                        	    a_type),
                          m_ssl_ctx(NULL),
                          m_ssl(NULL),
                          m_ssl_opt_verify(false),
                          m_ssl_opt_verify_allow_self_signed(false),
                          m_ssl_opt_tlsext_hostname(""),
                          m_ssl_opt_ca_file(""),
                          m_ssl_opt_ca_path(""),
                          m_ssl_opt_options(0),
                          m_ssl_opt_cipher_str(""),
                          m_ssl_state(SSL_STATE_FREE)
          {
                m_scheme = SCHEME_SSL;
          };

        // Destructor
        ~nconn_ssl()
        {
        };


        // TODO REMOVE!!!
#if 0
        int32_t run_state_machine(evr_loop *a_evr_loop, const host_info_t &a_host_info);
        int32_t send_request(bool is_reuse = false);
#endif

        int32_t set_opt(uint32_t a_opt, const void *a_buf, uint32_t a_len);
        int32_t get_opt(uint32_t a_opt, void **a_buf, uint32_t *a_len);
        bool is_listening(void) {return (m_ssl_state == SSL_STATE_LISTENING);};

        bool is_connecting(void) {return ((m_ssl_state == SSL_STATE_CONNECTING) ||
                                          (m_ssl_state == SSL_STATE_SSL_CONNECTING) ||
                                          (m_ssl_state == SSL_STATE_SSL_CONNECTING_WANT_READ) ||
                                          (m_ssl_state == SSL_STATE_SSL_CONNECTING_WANT_WRITE));};
        bool is_free(void) { return (m_ssl_state == SSL_STATE_FREE);}

        // TODO Experimental refactoring
        int32_t ncsetup(evr_loop *a_evr_loop);
        int32_t ncread(char *a_buf, uint32_t a_buf_len);
        int32_t ncwrite(char *a_buf, uint32_t a_buf_len);
        int32_t ncaccept(void);
        int32_t ncconnect(evr_loop *a_evr_loop);
        int32_t nccleanup(void);

        // -------------------------------------------------
        // Public static methods
        // -------------------------------------------------


        // -------------------------------------------------
        // Public members
        // -------------------------------------------------
private:

        // ---------------------------------------
        // Connection state
        // ---------------------------------------
        typedef enum ssl_state
        {
                SSL_STATE_FREE = 0,
                SSL_STATE_LISTENING,
                SSL_STATE_CONNECTING,

                // SSL
                SSL_STATE_SSL_CONNECTING,
                SSL_STATE_SSL_CONNECTING_WANT_READ,
                SSL_STATE_SSL_CONNECTING_WANT_WRITE,

                SSL_STATE_CONNECTED,
                SSL_STATE_READING,
                SSL_STATE_WRITING,
                SSL_STATE_DONE
        } ssl_state_t;

        // -------------------------------------------------
        // Private methods
        // -------------------------------------------------
        DISALLOW_COPY_AND_ASSIGN(nconn_ssl)

        int32_t ssl_connect(void);

        // TODO REMOVE!!!
#if 0
        int32_t receive_response(void);
#endif

        int32_t init(void);
        int32_t validate_server_certificate(const char* a_host, bool a_disallow_self_signed);

        // -------------------------------------------------
        // Private members
        // -------------------------------------------------
        // ssl
        SSL_CTX * m_ssl_ctx;
        SSL *m_ssl;
        bool m_ssl_opt_verify;
        bool m_ssl_opt_verify_allow_self_signed;
        std::string m_ssl_opt_tlsext_hostname;
        std::string m_ssl_opt_ca_file;
        std::string m_ssl_opt_ca_path;
        long m_ssl_opt_options;
        std::string m_ssl_opt_cipher_str;

        ssl_state_t m_ssl_state;

protected:
        // -------------------------------------------------
        // Protected methods
        // -------------------------------------------------
        int32_t set_listening(evr_loop *a_evr_loop, int32_t a_val);
        int32_t set_connected(evr_loop *a_evr_loop, int a_fd);

};

} //namespace ns_hlx {

#endif