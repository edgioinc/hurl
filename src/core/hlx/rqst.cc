//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//: All Rights Reserved
//:
//: \file:    rqst.cc
//: \details: TODO
//: \author:  Reed P. Morrison
//: \date:    07/20/2015
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

//: ----------------------------------------------------------------------------
//: Includes
//: ----------------------------------------------------------------------------
#include "hlx/hlx.h"
#include "http_parser/http_parser.h"
#include "nbq.h"
#include "ndebug.h"

#include <string.h>

namespace ns_hlx {

//: ----------------------------------------------------------------------------
//: \details: TODO
//: \return:  TODO
//: \param:   TODO
//: ----------------------------------------------------------------------------
rqst::rqst(void):
        hmsg(),
        m_p_url(),
        m_method(),
        m_uri_parsed(false),
        m_uri(),
        m_uri_path(),
        m_uri_query(),
        m_uri_fragment()
{
        m_type = hmsg::TYPE_RQST;
}

//: ----------------------------------------------------------------------------
//: \details: TODO
//: \return:  TODO
//: \param:   TODO
//: ----------------------------------------------------------------------------
rqst::~rqst(void)
{

}

//: ----------------------------------------------------------------------------
//: \details: TODO
//: \return:  TODO
//: \param:   TODO
//: ----------------------------------------------------------------------------
void rqst::clear(void)
{
        hmsg::clear();
        m_p_url.clear();
        m_method = 0;
        m_uri.clear();
        m_uri_path.clear();
        m_uri_query.clear();
        m_uri_fragment.clear();
        m_uri_parsed = false;
}

//: ----------------------------------------------------------------------------
//:                               Getters
//: ----------------------------------------------------------------------------
const std::string &rqst::get_uri_path()
{
        if(!m_uri_parsed)
        {
                int32_t l_status = parse_uri();
                if(l_status != STATUS_OK)
                {
                        // do nothing...
                }
        }
        return m_uri_path;
}

//: ----------------------------------------------------------------------------
//:                               Setters
//: ----------------------------------------------------------------------------


//: ----------------------------------------------------------------------------
//: \details: TODO
//: \return:  TODO
//: \param:   TODO
//: ----------------------------------------------------------------------------
int32_t rqst::parse_uri()
{
        if(m_uri_parsed)
        {
                return STATUS_OK;
        }

        // Copy out the url...
        // TODO zero copy???
        if(m_q && m_p_url.m_off && m_p_url.m_len)
        {
                char *l_path = NULL;
                l_path = copy_part(*m_q, m_p_url.m_off, m_p_url.m_len);
                if(l_path && strlen(l_path))
                {
                        m_uri = l_path;
                        free(l_path);
                }
        }

        std::string l_url_fixed = m_uri;
        // Find scheme prefix "://"
        if(m_uri.find("://", 0) == std::string::npos)
        {
                l_url_fixed = "http://bloop.com" + m_uri;
        }
        http_parser_url l_url;
        http_parser_url_init(&l_url);
        // silence bleating memory sanitizers...
        //memset(&l_url, 0, sizeof(l_url));
        int l_status;
        l_status = http_parser_parse_url(l_url_fixed.c_str(), l_url_fixed.length(), 0, &l_url);
        if(l_status != 0)
        {
                NDBG_PRINT("Error parsing url: %s\n", l_url_fixed.c_str());
                // TODO get error msg from http_parser
                return STATUS_ERROR;
        }

        for(uint32_t i_part = 0; i_part < UF_MAX; ++i_part)
        {
                //NDBG_PRINT("i_part: %d offset: %d len: %d\n", i_part, l_url.field_data[i_part].off, l_url.field_data[i_part].len);
                //NDBG_PRINT("len+off: %d\n",       l_url.field_data[i_part].len + l_url.field_data[i_part].off);
                //NDBG_PRINT("a_url.length(): %d\n", (int)a_url.length());
                if(l_url.field_data[i_part].len &&
                  // TODO Some bug with parser -parsing urls like "http://127.0.0.1" sans paths
                  ((l_url.field_data[i_part].len + l_url.field_data[i_part].off) <= l_url_fixed.length()))
                {
                        switch(i_part)
                        {
                        case UF_PATH:
                        {
                                m_uri_path = l_url_fixed.substr(l_url.field_data[i_part].off, l_url.field_data[i_part].len);
                                //NDBG_PRINT("l_part[UF_PATH]: %s\n", m_uri_path.c_str());
                                break;
                        }
                        case UF_QUERY:
                        {
                                m_uri_query = l_url_fixed.substr(l_url.field_data[i_part].off, l_url.field_data[i_part].len);
                                //NDBG_PRINT("l_part[UF_QUERY]: %s\n", m_uri_query.c_str());
                                break;
                        }
                        case UF_FRAGMENT:
                        {
                                m_uri_fragment = l_url_fixed.substr(l_url.field_data[i_part].off, l_url.field_data[i_part].len);
                                //NDBG_PRINT("l_part[UF_FRAGMENT]: %s\n", m_uri_fragment.c_str());
                                break;
                        }
                        case UF_USERINFO:
                        case UF_SCHEMA:
                        case UF_HOST:
                        case UF_PORT:
                        default:
                        {
                                break;
                        }
                        }
                }
        }
        m_uri_parsed = true;
        return STATUS_OK;
}

//: ----------------------------------------------------------------------------
//:                               Debug
//: ----------------------------------------------------------------------------
//: ----------------------------------------------------------------------------
//: \details: TODO
//: \return:  TODO
//: \param:   TODO
//: ----------------------------------------------------------------------------
void rqst::show(void)
{
        cr_list_t::const_iterator i_k = m_p_h_list_key.begin();
        cr_list_t::const_iterator i_v = m_p_h_list_val.begin();
        for(;i_k != m_p_h_list_key.end() && i_v != m_p_h_list_val.end(); ++i_k, ++i_v)
        {
                print_part(*m_q, i_k->m_off, i_k->m_len);
                NDBG_OUTPUT(": ");
                print_part(*m_q, i_v->m_off, i_v->m_len);
                NDBG_OUTPUT("\r\n");
        }
        NDBG_OUTPUT("\r\n");
        print_part(*m_q, m_p_body.m_off, m_p_body.m_len);
        NDBG_OUTPUT("\r\n");
}

} //namespace ns_hlx {
