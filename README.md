<!--
#
# Copyright © 2018 Moxa Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
-->

# AWS-IOT-SDK-CPP

> **Description:**
>
> Cloud: Create device instance for connecting by SDK program
>
> Host: Cross-compiling the SDK
>
> Target: Executing the SDK program

## Cloud (AWS)

### Sign in to Cloud

* Sign in to [AWS IoT Cloud][cloud]. If you do not have an account, please register a new one.

### Create Device

1. In the left navigation pane, expand **Manage** and then choose **Things**. On the page that says **You don't have any things yet**, choose **Register a thing**.

    ![create_device_01][create_device_01]

2. On the **Creating AWS IoT things** page, choose **Create a single thing**.

    ![create_device_02][create_device_02]

3. On the **Add your device to the thing registry** page, fill in the necessary information then choose **Next**.

    ![create_device_03][create_device_03]
    ![create_device_04][create_device_04]

4. On the **Add a certificate for your thing** page, choose **Create thing without certificate**.

    ![create_device_05][create_device_05]

5. Finish creating the device

    ![create_device_06][create_device_06]

### Create Policy

1. In the left navigation pane, expand **Secure** and then choose **Policies**. On the page that says **You don't have any policies yet**, choose **Create a policy**.

    ![create_policy_01][create_policy_01]

2. On the **Create a policy** page, fill in the necessary information and select the **Allow** check box then choose **Create**.

    ![create_policy_02][create_policy_02]
    ![create_policy_03][create_policy_03]

4. Finish creating the policy

    ![create_policy_04][create_policy_04]

### Create Certificate

1. In the left navigation pane, expand **Secure** and then choose **Certificates**. On the page that says **You don't have any certificates yet**, choose **Create a certificate**.

    ![create_certificate_01][create_certificate_01]

2. On the **Create a certificate** page, choose **Create certificate**.

    ![create_certificate_02][create_certificate_02]

3. On the **Certificate created!** page, choose **Download** for the **certificate**, **private key**, and the **root CA** (the public key need not be downloaded). Save each of them to your computer, and then choose **Done**.

    ![create_certificate_03][create_certificate_03]
    * For downloading the **root CA** for AWS IoT, on the **Server Authentication** page, choose **Amazon Root CA 1**.

    ![create_certificate_04][create_certificate_04]

4. Finish creating the certificate

    ![create_certificate_05][create_certificate_05]

### Active Certificate

1. In the left navigation pane, expand **Secure** and then choose **Certificates**. In the box for the certificate you created before, choose **...** to open a drop-down menu, and then choose **Attach thing**.

    ![active_certificate_01][active_certificate_01]

2. In the **Attach things to certificate(s)** dialog box, select the check box next to the device you created before, and then choose **Attach**.

    ![active_certificate_02][active_certificate_02]

3. In the left navigation pane, expand **Secure** and then choose **Certificates**. In the box for the certificate you created before, choose **...** to open a drop-down menu, and then choose **Attach policy**.

    ![active_certificate_01][active_certificate_03]

4. In the **Attach policies to certificate(s)** dialog box, select the check box next to the policy you created before, and then choose **Attach**.

    ![active_certificate_02][active_certificate_04]

5. In the left navigation pane, expand **Secure** and then choose **Certificates**. In the box for the certificate you created before, choose **...** to open a drop-down menu, and then choose **Activate**.

    ![active_certificate_05][active_certificate_05]

6. Finish activating the certificate

    ![active_certificate_06][active_certificate_06]

### Copy Device Endpoint

* In the left navigation pane, choose **Settings**. On the **Custom endpoint** item, you can found the **Endpoint** that allows you to connect to AWS IoT.

    ![copy_device_endpoint][copy_device_endpoint]

### View Device Messages

1. In the left navigation pane, choose **Test**. On the **Subscribe** item, fill in the necessary information then choose **Subscribe to topic**.

    ![view_device_messages_01][view_device_messages_01]

2. Device messages result

    ![view_device_messages_02][view_device_messages_02]
    ![view_device_messages_03][view_device_messages_03]

## Host (x86_64-linux)

### Setup the Environment

1. Setup a network connection to allow host able to access the network.

2. Install GNU cross-toolchain provide by MOXA.

3. Install following package from package manager.

    ```
    cmake git rsync
    ```

### Build the SDK

1. Setup dependencies and SDK to output directory.

    ```
    $ ./setup.sh
    ```
    * For more setup.sh options.

    ```
    $ ./setup.sh --help

    Usage: ./setup.sh [options]

    Options:
        -git                Git repository of SDK.
                            Default: https://github.com/aws/aws-iot-device-sdk-cpp.git

        -ver                Version of SDK.
                            Default: v1.4.0

        --toolchain         GNU cross-toolchain directory.
                            Default: /usr/local/bin/gcc-linaro-5.1-2015.08-x86_64_arm-linux-gnueabihf

        --help              Display this help and exit.

    Examples:
        Default             ./setup.sh
        Specify             ./setup.sh -git https://github.com/aws/aws-iot-device-sdk-cpp.git -ver v1.4.0
                            ./setup.sh --toolchain /usr/local/bin/gcc-linaro-5.1-2015.08-x86_64_arm-linux-gnueabihf
    ```

2. Copy the **certificate**, **private key**, and the **root CA** that downloaded from the cloud to the following directory. [[Download Certificate](#create-certificate)]

    ```
    $ tree output/sdk_aws/certs
    output/sdk_aws/certs
    ├── abd17825b2-certificate.pem.crt
    ├── abd17825b2-private.pem.key
    └── AmazonRootCA1.pem
    ```

3. Add the **endpoint** and the path of **certificate**, **private key**, and the **root CA** to **SampleConfig.json** file. [[Copy Device Endpoint](#copy-device-endpoint)]

    ```
    $ vim output/sdk_aws/common/SampleConfig.json
    ```
    ```
    {
        "endpoint": "example.amazonaws.com",
        "mqtt_port": 8883,
        "https_port": 443,
        "greengrass_discovery_port": 8443,
        "root_ca_relative_path": "certs/AmazonRootCA1.pem",
        "device_certificate_relative_path": "certs/abd17825b2-certificate.pem.crt",
        "device_private_key_relative_path": "certs/abd17825b2-private.pem.key",
        "tls_handshake_timeout_msecs": 60000,
        "tls_read_timeout_msecs": 2000,
        "tls_write_timeout_msecs": 2000,
        "aws_region": "",
        "aws_access_key_id": "",
        "aws_secret_access_key": "",
        "aws_session_token": "",
        "client_id": "CppSDKTesting",
        "thing_name": "CppSDKTesting",
        "is_clean_session": true,
        "mqtt_command_timeout_msecs": 20000,
        "keepalive_interval_secs": 600,
        "minimum_reconnect_interval_secs": 1,
        "maximum_reconnect_interval_secs": 128,
        "maximum_acks_to_wait_for": 32,
        "action_processing_rate_hz": 5,
        "maximum_outgoing_action_queue_length": 32,
        "discover_action_timeout_msecs": 300000
    }
    ```

4. Build the whole SDK.

    ```
    $ ./build.sh
    ```
    * All compiled SDK program can be found in the following directory, including example **pub-sub-sample**.

    ```
    $ tree output/sdk_aws/build_cmake/bin
    output/sdk_aws/build_cmake/bin
    ├── aws-iot-integration-tests
    ├── aws-iot-unit-tests
    ├── certs
    │   ├── abd17825b2-certificate.pem.crt
    │   ├── abd17825b2-private.pem.key
    │   └── AmazonRootCA1.pem
    ├── config
    │   ├── IntegrationTestConfig.json
    │   └── SampleConfig.json
    ├── jobs-sample
    ├── pub-sub-sample
    ├── shadow-delta-sample
    └── TestParser.json
    ```

* Note

    ```
    In general, the setup.sh only needs to be executed once.
    The build.sh needs to be executed after any code change of the SDK.
    ```

## Target (arm-linux)

### Setup the Environment

1. Setup a network connection to allow target able to access the network.

2. Copy compiled SDK program from host to target

    ```
    $ tree
    .
    ├── certs
    │   ├── abd17825b2-certificate.pem.crt
    │   ├── abd17825b2-private.pem.key
    │   └── AmazonRootCA1.pem
    ├── config
    │   └── SampleConfig.json
    └── pub-sub-sample
    ```

### Execute the SDK

1. Execute SDK program that cross-compiled by host.

    ```
    $ ./pub-sub-sample
    ```
    * You need to install the dependency library for the SDK program if any not found.

2. [View device messages on cloud](#view-device-messages).

## Reference

[1] [https://github.com/aws/aws-iot-device-sdk-cpp][Reference_01]

[2] [https://docs.aws.amazon.com/iot/latest/developerguide/what-is-aws-iot.html][Reference_02]

[comment]: # (Images)
[create_device_01]: readme/create_device_01.png
[create_device_02]: readme/create_device_02.png
[create_device_03]: readme/create_device_03.png
[create_device_04]: readme/create_device_04.png
[create_device_05]: readme/create_device_05.png
[create_device_06]: readme/create_device_06.png

[create_policy_01]: readme/create_policy_01.png
[create_policy_02]: readme/create_policy_02.png
[create_policy_03]: readme/create_policy_03.png
[create_policy_04]: readme/create_policy_04.png

[create_certificate_01]: readme/create_certificate_01.png
[create_certificate_02]: readme/create_certificate_02.png
[create_certificate_03]: readme/create_certificate_03.png
[create_certificate_04]: readme/create_certificate_04.png
[create_certificate_05]: readme/create_certificate_05.png

[active_certificate_01]: readme/active_certificate_01.png
[active_certificate_02]: readme/active_certificate_02.png
[active_certificate_03]: readme/active_certificate_03.png
[active_certificate_04]: readme/active_certificate_04.png
[active_certificate_05]: readme/active_certificate_05.png
[active_certificate_06]: readme/active_certificate_06.png

[copy_device_endpoint]: readme/copy_device_endpoint.png

[view_device_messages_01]: readme/view_device_messages_01.png
[view_device_messages_02]: readme/view_device_messages_02.png
[view_device_messages_03]: readme/view_device_messages_03.png

[comment]: # (Links)
[cloud]: https://console.aws.amazon.com/iot/home
[Reference_01]: https://github.com/aws/aws-iot-device-sdk-cpp
[Reference_02]: https://docs.aws.amazon.com/iot/latest/developerguide/what-is-aws-iot.html
