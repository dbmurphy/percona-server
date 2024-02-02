/* Copyright (c) 2022 Percona LLC and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 2 of
   the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA */
#include <mysql/plugin_auth.h>
#include <aws/core/Aws.h>
#include <aws/sts/STSClient.h>
#include <aws/sts/model/GetCallerIdentityRequest.h>
#include <aws/core/auth/AWSCredentialsProvider.h>

class AwsSdkInitializer {
public:
    AwsSdkInitializer() { Aws::InitAPI(options); }
    ~AwsSdkInitializer() { Aws::ShutdownAPI(options); }
private:
    Aws::SDKOptions options;
};

static AwsSdkInitializer awsSdkInitializer;

extern "C" int auth_awsiam_authenticate(MYSQL_PLUGIN_VIO *vio, MYSQL_SERVER_AUTH_INFO *info) {
    unsigned char *pkt;
    int pkt_len;

    const char* iam_role = info->user_name;
    if (vio->read_packet(vio, &pkt, &pkt_len)) return CR_ERROR;
    std::string token(reinterpret_cast<char*>(pkt), pkt_len);

    Aws::Client::ClientConfiguration clientConfig;
    Aws::STS::STSClient sts(clientConfig);
    Aws::STS::Model::GetCallerIdentityRequest request;
    Aws::Auth::AWSCredentials credentials("", "", token);
    sts.SetAWSAuthSigner(std::make_shared<Aws::Auth::DefaultAuthSigner>(credentials));
    auto outcome = sts.GetCallerIdentity(request);

    if (outcome.IsSuccess() && outcome.GetResult().GetArn().find(iam_role) != std::string::npos) {
        return CR_OK;
    } else {
        return CR_ERROR;
    }
}

static struct st_mysql_auth auth_awsiam_handler = {
    MYSQL_AUTHENTICATION_INTERFACE_VERSION,
    auth_awsiam_authenticate
};

mysql_declare_plugin(auth_awsiam) {
    MYSQL_AUTHENTICATION_PLUGIN,
    &auth_awsiam_handler,
    "auth_awsiam",
    "David Murphy",
    "AWS IAM Authentication Plugin",
    PLUGIN_LICENSE_GPL,
    NULL, /* Plugin init */
    NULL, /* Plugin check uninstall */
    NULL, /* Plugin deinit */
    0x0100, /* Version = 1.0 */
    NULL, /* status variables */
    NULL, /* system variables */
    NULL, /* config options */
    0, /* flags */
}
mysql_declare_plugin_end;
