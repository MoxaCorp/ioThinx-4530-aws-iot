// Copyright (C) 2019 Moxa Inc. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

// Include ioThinx I/O Library
#include <iothinx/iothinxio.h>
// Include AWS Library
#include "util/logging/ConsoleLogSystem.hpp"
#include "util/logging/Logging.hpp"
#include "util/logging/LogMacros.hpp"
#include "ConfigCommon.hpp"
#include "OpenSSLConnection.hpp"
#include "moxa_sample_mqtt.hpp"

#define LOG_TAG_MOXA_SAMPLE             "[Sample - MoxaSampleMqtt]"
#define MOXA_SAMPLE_TOPIC               "sdk/test/cpp"
#define DEVICEID_KEY                    "DeviceId"
#define DEVICEID_VAL                    "ioThinx"
#define DI_VALUES_KEY                   "DI_Values"
#define DO_VALUES_KEY                   "DO_Values"
#define DO_SETVALUES_KEY                "DO_SetValues"
#define SHADOW_DOCUMENT_STRING_SEND     \
    "{"                                 \
        "\"DeviceId\":\"ioThinx\","     \
        "\"DI_Values\":0,"              \
        "\"DO_Values\":0"               \
    "}"

namespace awsiotsdk {
    namespace samples {
        /*
         * MoxaSampleMqtt::SubscribeCallback:
         *   - Set DO value to I/O module when a message is received on a subscribed topic.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         * Parameter:
         *   - topic_name           : Topic name for this subscription.
         *   - payload              : MQTT message payload.
         *   - p_app_handler_data   : Context data for the subscription handler.
         */
        ResponseCode MoxaSampleMqtt::SubscribeCallback(util::String topic_name,
                                                       util::String payload,
                                                       std::shared_ptr<mqtt::SubscriptionHandlerContextData> p_app_handler_data)
        {
            ResponseCode aws_rc = ResponseCode::SUCCESS;
            int io_rc = 0;
            util::String DeviceId;
            util::JsonDocument recv;
            uint32_t slot = 1;
            uint32_t do_values = 0;

            // AWS: Initialize JsonDocument from JsonString.
            aws_rc = util::JsonParser::InitializeFromJsonString(recv, payload);
            if (aws_rc != ResponseCode::SUCCESS)
            {
                AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                              "InitializeFromJsonString() = %s",
                              ResponseHelper::ToString(aws_rc).c_str());
                return aws_rc;
            }

            std::cout << "****************************************" << std::endl
                      << "Subscribe : " << topic_name << std::endl
                      << "Recv Data : " << payload << std::endl
                      << "****************************************" << std::endl;

            // AWS: Get Device ID.
            aws_rc = util::JsonParser::GetStringValue(recv, DEVICEID_KEY, DeviceId);
            if (aws_rc != ResponseCode::SUCCESS)
            {
                return aws_rc;
            }

            // AWS: Check Device ID.
            if (DeviceId.compare(DEVICEID_VAL) != 0)
            {
                return ResponseCode::FAILURE;
            }

            // AWS: Get DO value.
            aws_rc = util::JsonParser::GetUint32Value(recv, DO_SETVALUES_KEY, do_values);
            if (aws_rc != ResponseCode::SUCCESS)
            {
                return aws_rc;
            }

            // ioThinx: Set DO value.
            io_rc = ioThinx_DO_SetValues(slot, do_values);
            if (io_rc != IOTHINX_ERR_OK)
            {
                AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                              "ioThinx_DO_SetValues() = %d",
                              io_rc);
                return ResponseCode::FAILURE;
            }

            std::cout << DO_SETVALUES_KEY << " : SUCCESS." << std::endl;

            return ResponseCode::SUCCESS;
        }

        /*
         * MoxaSampleMqtt::DisconnectCallback:
         *   - Show result when a disconnect occurs.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         * Parameter:
         *   - client_id            : Client ID use to make the connection.
         *   - p_app_handler_data   : Context data for the disconnect handler.
         */
        ResponseCode MoxaSampleMqtt::DisconnectCallback(util::String client_id,
                                                        std::shared_ptr<DisconnectCallbackContextData> p_app_handler_data)
        {
            std::cout << "****************************************" << std::endl
                      << client_id << " Disconnected!" << std::endl
                      << "****************************************" << std::endl;
            return ResponseCode::SUCCESS;
        }

        /*
         * MoxaSampleMqtt::ReconnectCallback:
         *   - Show result when a reconnect occurs.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         * Parameter:
         *   - client_id            : Client ID use to make the connection.
         *   - p_app_handler_data   : Context data for the reconnect handler.
         *   - reconnect_result     : ResponseCode of the reconnect.
         */
        ResponseCode MoxaSampleMqtt::ReconnectCallback(util::String client_id,
                                                       std::shared_ptr<ReconnectCallbackContextData> p_app_handler_data,
                                                       ResponseCode reconnect_result)
        {
            std::cout << "****************************************" << std::endl
                      << client_id << " Reconnect Attempted. Result " << ResponseHelper::ToString(reconnect_result) << std::endl
                      << "****************************************" << std::endl;
            return ResponseCode::SUCCESS;
        }

        /*
         * MoxaSampleMqtt::ResubscribeCallback:
         *   - Show result when a resubscribe occurs.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         * Parameter:
         *   - client_id            : Client ID use to make the connection.
         *   - p_app_handler_data   : Context data for the resubscribe handler.
         *   - resubscribe_result   : ResponseCode of the resubscribe.
         */
        ResponseCode MoxaSampleMqtt::ResubscribeCallback(util::String client_id,
                                                         std::shared_ptr<ResubscribeCallbackContextData> p_app_handler_data,
                                                         ResponseCode resubscribe_result)
        {
            std::cout << "****************************************" << std::endl
                      << client_id << " Resubscribe Attempted. Result " << ResponseHelper::ToString(resubscribe_result) << std::endl
                      << "****************************************" << std::endl;
            return ResponseCode::SUCCESS;
        }

        /*
         * MoxaSampleMqtt::PublishMqtt:
         *   - Performs a MQTT Publish operation in Async mode.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         * Parameter:
         *   - payload      : MQTT message payload.
         *   - packet_id    : Packet ID of the message being sent.
         */
        ResponseCode MoxaSampleMqtt::PublishMqtt(util::String &payload,
                                                 uint16_t &packet_id)
        {
            ResponseCode rc = ResponseCode::SUCCESS;
            util::String p_topic_name_str = MOXA_SAMPLE_TOPIC;
            std::unique_ptr<Utf8String> p_topic_name = Utf8String::Create(p_topic_name_str);

            // AWS: Performs a MQTT Publish operation in Async mode.
            rc = p_iot_client_->PublishAsync(std::move(p_topic_name), false, false,
                                             mqtt::QoS::QOS1, payload, nullptr, packet_id);
            if (rc != ResponseCode::ACTION_QUEUE_FULL)
            {
                return rc;
            }

            // Sleep 1 second.
            std::this_thread::sleep_for(std::chrono::seconds(1));

            return rc;
        }

        /*
         * MoxaSampleMqtt::UnsubscribeMqtt:
         *   - Performs a MQTT Unsubscribe operation in blocking mode.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         */
        ResponseCode MoxaSampleMqtt::UnsubscribeMqtt(void)
        {
            util::String p_topic_name_str = MOXA_SAMPLE_TOPIC;
            std::unique_ptr<Utf8String> p_topic_name = Utf8String::Create(p_topic_name_str);
            util::Vector<std::unique_ptr<Utf8String>> topic_vector;
            topic_vector.push_back(std::move(p_topic_name));

            // AWS: Performs a MQTT Unsubscribe operation in blocking mode.
            return p_iot_client_->Unsubscribe(std::move(topic_vector), ConfigCommon::mqtt_command_timeout_);
        }

        /*
         * MoxaSampleMqtt::SubscribeMqtt:
         *   - Performs a MQTT Subscribe operation in blocking mode.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         */
        ResponseCode MoxaSampleMqtt::SubscribeMqtt(void)
        {
            util::String p_topic_name_str = MOXA_SAMPLE_TOPIC;
            std::unique_ptr<Utf8String> p_topic_name = Utf8String::Create(p_topic_name_str);
            util::Vector<std::shared_ptr<mqtt::Subscription>> topic_vector;

            mqtt::Subscription::ApplicationCallbackHandlerPtr
                p_sub_handler = std::bind(&MoxaSampleMqtt::SubscribeCallback,
                                          this,
                                          std::placeholders::_1,
                                          std::placeholders::_2,
                                          std::placeholders::_3);
            std::shared_ptr<mqtt::Subscription>
                p_subscription = mqtt::Subscription::Create(std::move(p_topic_name), mqtt::QoS::QOS0, p_sub_handler, nullptr);
            topic_vector.push_back(p_subscription);

            // AWS: Performs a MQTT Subscribe operation in blocking mode.
            return p_iot_client_->Subscribe(topic_vector, ConfigCommon::mqtt_command_timeout_);
        }

        /*
         * MoxaSampleMqtt::DisconnectMqtt:
         *   - Performs a Network and MQTT Disconnect operation in blocking mode.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         */
        ResponseCode MoxaSampleMqtt::DisconnectMqtt(void)
        {
            // AWS: Performs a Network and MQTT Disconnect operation in blocking mode.
            return p_iot_client_->Disconnect(ConfigCommon::mqtt_command_timeout_);
        }

        /*
         * MoxaSampleMqtt::ConnectMqtt:
         *   - Performs a Network and MQTT Connect operation in blocking mode.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         */
        ResponseCode MoxaSampleMqtt::ConnectMqtt(void)
        {
            ResponseCode rc = ResponseCode::SUCCESS;

            std::unique_ptr<Utf8String>
                p_client_id = Utf8String::Create(ConfigCommon::base_client_id_);

            // AWS: Performs a Network and MQTT Connect operation in blocking mode.
            rc = p_iot_client_->Connect(ConfigCommon::mqtt_command_timeout_, ConfigCommon::is_clean_session_,
                                        mqtt::Version::MQTT_3_1_1, ConfigCommon::keep_alive_timeout_secs_,
                                        std::move(p_client_id), nullptr, nullptr, nullptr);
            if (rc != ResponseCode::MQTT_CONNACK_CONNECTION_ACCEPTED)
            {
                return rc;
            }

            return ResponseCode::SUCCESS;
        }

        /*
         * MoxaSampleMqtt::CreateMqtt:
         *   - Create factory method, with additional parameters for disconnect, reconnect and resubscribe callbacks.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         */
        ResponseCode MoxaSampleMqtt::CreateMqtt(void)
        {
            ClientCoreState::ApplicationDisconnectCallbackPtr
                p_disconnect_handler = std::bind(&MoxaSampleMqtt::DisconnectCallback,
                                                 this,
                                                 std::placeholders::_1,
                                                 std::placeholders::_2);

            ClientCoreState::ApplicationReconnectCallbackPtr
                p_reconnect_handler = std::bind(&MoxaSampleMqtt::ReconnectCallback,
                                                this,
                                                std::placeholders::_1,
                                                std::placeholders::_2,
                                                std::placeholders::_3);

            ClientCoreState::ApplicationResubscribeCallbackPtr
                p_resubscribe_handler = std::bind(&MoxaSampleMqtt::ResubscribeCallback,
                                                  this,
                                                  std::placeholders::_1,
                                                  std::placeholders::_2,
                                                  std::placeholders::_3);

            // AWS: Create MQTT client instance.
            p_iot_client_ = std::shared_ptr<MqttClient>(MqttClient::Create(p_network_connection_,
                                                                           ConfigCommon::mqtt_command_timeout_,
                                                                           p_disconnect_handler, nullptr,
                                                                           p_reconnect_handler, nullptr,
                                                                           p_resubscribe_handler, nullptr));
            if (p_iot_client_ == nullptr)
            {
                return ResponseCode::FAILURE;
            }

            return ResponseCode::SUCCESS;
        }

        /*
         * MoxaSampleMqtt::InitializeNetwork:
         *   - Initialize network with the OpenSSL object.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         */
        ResponseCode MoxaSampleMqtt::InitializeNetwork(void)
        {
            ResponseCode rc = ResponseCode::SUCCESS;

            std::shared_ptr<network::OpenSSLConnection>
                p_network_connection = std::make_shared<network::OpenSSLConnection>(ConfigCommon::endpoint_,
                                                                                    ConfigCommon::endpoint_mqtt_port_,
                                                                                    ConfigCommon::root_ca_path_,
                                                                                    ConfigCommon::client_cert_path_,
                                                                                    ConfigCommon::client_key_path_,
                                                                                    ConfigCommon::tls_handshake_timeout_,
                                                                                    ConfigCommon::tls_read_timeout_,
                                                                                    ConfigCommon::tls_write_timeout_, true);

            // AWS: Initialize the OpenSSL object.
            rc = p_network_connection->Initialize();
            if (rc != ResponseCode::SUCCESS)
            {
                return rc;
            }

            p_network_connection_ = std::dynamic_pointer_cast<NetworkConnection>(p_network_connection);

            return ResponseCode::SUCCESS;
        }

        /*
         * MoxaSampleMqtt::RunSample:
         *   - Establish connection between the device and the AWS IoT.
         * Return:
         *   - ResponseCode::SUCCESS on success or any other refer to the ResponseCode.
         */
        ResponseCode MoxaSampleMqtt::RunSample(void)
        {
            ResponseCode aws_rc = ResponseCode::SUCCESS;
            int io_rc = 0;
            uint16_t packet_id = 0;
            uint32_t slot = 1;
            uint32_t di_values = 0;
            uint32_t do_values = 0;
            uint32_t DI_Values = 0;
            uint32_t DO_Values = 0;
            util::String payload;
            util::JsonDocument send;

            // ioThinx: Initialize I/O.
            io_rc = ioThinx_IO_Client_Init();
            if (io_rc != IOTHINX_ERR_OK)
            {
                AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                              "ioThinx_IO_Client_Init() = %d",
                              io_rc);
                return ResponseCode::FAILURE;
            }

            // AWS: Initialize JsonDocument from JsonString.
            aws_rc = util::JsonParser::InitializeFromJsonString(send, SHADOW_DOCUMENT_STRING_SEND);
            if (aws_rc != ResponseCode::SUCCESS)
            {
                AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                              "InitializeFromJsonString() = %s",
                              ResponseHelper::ToString(aws_rc).c_str());
                return aws_rc;
            }

            // AWS: Initialize network.
            aws_rc = InitializeNetwork();
            if (aws_rc != ResponseCode::SUCCESS)
            {
                AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                              "InitializeNetwork() = %s",
                              ResponseHelper::ToString(aws_rc).c_str());
                return aws_rc;
            }

            // AWS: Create mqtt.
            aws_rc = CreateMqtt();
            if (aws_rc != ResponseCode::SUCCESS)
            {
                AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                              "CreateMqtt() = %s",
                              ResponseHelper::ToString(aws_rc).c_str());
                return aws_rc;
            }

            // AWS: Connect mqtt.
            aws_rc = ConnectMqtt();
            if (aws_rc != ResponseCode::SUCCESS)
            {
                AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                              "ConnectMqtt() = %s",
                              ResponseHelper::ToString(aws_rc).c_str());
                return aws_rc;
            }

            // AWS: Subscribe mqtt.
            aws_rc = SubscribeMqtt();
            if (aws_rc != ResponseCode::SUCCESS)
            {
                AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                              "SubscribeMqtt() = %s",
                              ResponseHelper::ToString(aws_rc).c_str());
                // AWS: Disconnect mqtt.
                DisconnectMqtt();
                return aws_rc;
            }

            // ioThinx: Get DI value.
            io_rc = ioThinx_DI_GetValues(slot, &di_values);
            if (io_rc != IOTHINX_ERR_OK)
            {
                AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                              "ioThinx_DI_GetValues() = %d",
                              io_rc);
                // AWS: Unsubscribe mqtt.
                UnsubscribeMqtt();
                // AWS: Disconnect mqtt.
                DisconnectMqtt();
                return ResponseCode::FAILURE;
            }

            // ioThinx: Get DO value.
            io_rc = ioThinx_DO_GetValues(slot, &do_values);
            if (io_rc != IOTHINX_ERR_OK)
            {
                AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                              "ioThinx_DO_GetValues() = %d",
                              io_rc);
                // AWS: Unsubscribe mqtt.
                UnsubscribeMqtt();
                // AWS: Disconnect mqtt.
                DisconnectMqtt();
                return ResponseCode::FAILURE;
            }

            // AWS: Set data.
            send[DI_VALUES_KEY] = di_values;
            send[DO_VALUES_KEY] = do_values;
            payload = util::JsonParser::ToString(send);

            // AWS: Publish mqtt.
            aws_rc = PublishMqtt(payload, packet_id);
            if (aws_rc != ResponseCode::SUCCESS && aws_rc != ResponseCode::ACTION_QUEUE_FULL)
            {
                AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                              "PublishMqtt() = %s",
                              ResponseHelper::ToString(aws_rc).c_str());
                // AWS: Unsubscribe mqtt.
                UnsubscribeMqtt();
                // AWS: Disconnect mqtt.
                DisconnectMqtt();
                return aws_rc;
            }

            std::cout << packet_id << " Send " << ResponseHelper::ToString(aws_rc) << std::endl;

            while (true)
            {
                // Sleep 1 second.
                std::this_thread::sleep_for(std::chrono::seconds(1));

                // ioThinx: Get current DI value.
                io_rc = ioThinx_DI_GetValues(slot, &di_values);
                if (io_rc != IOTHINX_ERR_OK)
                {
                    AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                                  "ioThinx_DI_GetValues() = %d",
                                  io_rc);
                    // AWS: Unsubscribe mqtt.
                    UnsubscribeMqtt();
                    // AWS: Disconnect mqtt.
                    DisconnectMqtt();
                    return ResponseCode::FAILURE;
                }

                // ioThinx: Get current DO value.
                io_rc = ioThinx_DO_GetValues(slot, &do_values);
                if (io_rc != IOTHINX_ERR_OK)
                {
                    AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                                  "ioThinx_DO_GetValues() = %d",
                                  io_rc);
                    // AWS: Unsubscribe mqtt.
                    UnsubscribeMqtt();
                    // AWS: Disconnect mqtt.
                    DisconnectMqtt();
                    return ResponseCode::FAILURE;
                }

                // AWS: Get previous DI value.
                aws_rc = util::JsonParser::GetUint32Value(send, DI_VALUES_KEY, DI_Values);
                if (aws_rc != ResponseCode::SUCCESS)
                {
                    AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                                  "GetUint32Value() = %s",
                                  ResponseHelper::ToString(aws_rc).c_str());
                    // AWS: Unsubscribe mqtt.
                    UnsubscribeMqtt();
                    // AWS: Disconnect mqtt.
                    DisconnectMqtt();
                    return aws_rc;
                }

                // AWS: Get previous DO value.
                aws_rc = util::JsonParser::GetUint32Value(send, DO_VALUES_KEY, DO_Values);
                if (aws_rc != ResponseCode::SUCCESS)
                {
                    AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                                  "GetUint32Value() = %s",
                                  ResponseHelper::ToString(aws_rc).c_str());
                    // AWS: Unsubscribe mqtt.
                    UnsubscribeMqtt();
                    // AWS: Disconnect mqtt.
                    DisconnectMqtt();
                    return aws_rc;
                }

                // If no value change of DI or DO.
                if (DI_Values == di_values && DO_Values == do_values)
                {
                    continue;
                }

                // AWS: Update data.
                send[DI_VALUES_KEY] = di_values;
                send[DO_VALUES_KEY] = do_values;
                payload = util::JsonParser::ToString(send);

                // AWS: Publish mqtt.
                aws_rc = PublishMqtt(payload, packet_id);
                if (aws_rc != ResponseCode::SUCCESS && aws_rc != ResponseCode::ACTION_QUEUE_FULL)
                {
                    AWS_LOG_ERROR(LOG_TAG_MOXA_SAMPLE,
                                  "PublishMqtt() = %s",
                                  ResponseHelper::ToString(aws_rc).c_str());
                    // AWS: Unsubscribe mqtt.
                    UnsubscribeMqtt();
                    // AWS: Disconnect mqtt.
                    DisconnectMqtt();
                    return aws_rc;
                }

                std::cout << packet_id << " Send " << ResponseHelper::ToString(aws_rc) << std::endl;
            }

            // AWS: Unsubscribe mqtt.
            UnsubscribeMqtt();
            // AWS: Disconnect mqtt.
            DisconnectMqtt();
            return ResponseCode::SUCCESS;
        }
    }
}

int main(int argc, char const *argv[])
{
    awsiotsdk::ResponseCode rc = awsiotsdk::ResponseCode::SUCCESS;

    std::shared_ptr<awsiotsdk::util::Logging::ConsoleLogSystem>
        p_log_system = std::make_shared<awsiotsdk::util::Logging::ConsoleLogSystem>(awsiotsdk::util::Logging::LogLevel::Info);

    // AWS: Call this at the beginning of your program, prior to any AWS calls.
    awsiotsdk::util::Logging::InitializeAWSLogging(p_log_system);

    std::unique_ptr<awsiotsdk::samples::MoxaSampleMqtt>
        moxa_sample_mqtt = std::unique_ptr<awsiotsdk::samples::MoxaSampleMqtt>(new awsiotsdk::samples::MoxaSampleMqtt());

    // AWS: Initialize Config.
    rc = awsiotsdk::ConfigCommon::InitializeCommon("config/SampleConfig.json");
    if (rc != awsiotsdk::ResponseCode::SUCCESS)
    {
        // AWS: Call this at the exit point of your program, after all calls have finished.
        awsiotsdk::util::Logging::ShutdownAWSLogging();
        return static_cast<int>(rc);
    }

    // Run sample.
    rc = moxa_sample_mqtt->RunSample();

    // AWS: Call this at the exit point of your program, after all calls have finished.
    awsiotsdk::util::Logging::ShutdownAWSLogging();
    return static_cast<int>(rc);
}
