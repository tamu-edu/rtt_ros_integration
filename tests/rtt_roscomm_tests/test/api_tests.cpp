/** Copyright (c) 2013, Jonathan Bohren, all rights reserved. 
 * This software is released under the BSD 3-clause license, for the details of
 * this license, please see LICENSE.txt at the root of this repository. 
 */

#include <string>
#include <vector>
#include <iterator>

#include <rtt/os/startstop.h>

#include <ocl/DeploymentComponent.hpp>
#include <ocl/TaskBrowser.hpp>
#include <ocl/LoggingService.hpp>
#include <rtt/Logger.hpp>
#include <rtt/deployment/ComponentLoader.hpp>
#include <rtt/scripting/Scripting.hpp>

#include <rtt_roscomm/rtt_rostopic.h>
#include <std_msgs/typekit/String.h>

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include <gtest/gtest.h>

boost::shared_ptr<OCL::DeploymentComponent> deployer;
boost::shared_ptr<RTT::Scripting> scripting_service;

TEST(BasicTest, Import) 
{
  // Import rtt_ros plugin
  EXPECT_TRUE(RTT::ComponentLoader::Instance()->import("rtt_ros", "" ));
  EXPECT_TRUE(RTT::ComponentLoader::Instance()->import("rtt_roscomm", "" ));
}

TEST(BasicTest, ImportTypekit) 
{
  // Import rtt_std_msgs typekit
  EXPECT_TRUE(RTT::ComponentLoader::Instance()->import("rtt_std_msgs", "" ));
  EXPECT_TRUE(scripting_service->eval("var ConnPolicy float_out = ros.topic(\"float_out\")"));
}

TEST(BasicTest, OutOfBandTest)
{
  // Import plugins
  EXPECT_TRUE(RTT::ComponentLoader::Instance()->import("rtt_std_msgs", "" ));
  EXPECT_TRUE(RTT::ComponentLoader::Instance()->import("rtt_rosnode", "" ));

  RTT::OutputPort<std_msgs::String> out("out");
  RTT::InputPort<std_msgs::String> in("in");

  // Create an out-of-band connection with ROS transport (a publisher/subscriber pair)
  EXPECT_TRUE(out.connectTo(&in, rtt_roscomm::topicLatched("~talker")));

  std_msgs::String sample;
  sample.data = "Hello world!";
  out.write(sample);

  usleep(1000000);

  sample.data.clear();
  EXPECT_EQ(RTT::NewData, in.read(sample) );
  EXPECT_EQ("Hello world!", sample.data);
} 

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);

  // Initialize Orocos
  __os_init(argc, argv);

  deployer = boost::make_shared<OCL::DeploymentComponent>();
  scripting_service = deployer->getProvider<RTT::Scripting>("scripting");

  RTT::Logger::log().setStdStream(std::cerr);
  RTT::Logger::log().mayLogStdOut(true);
  RTT::Logger::log().setLogLevel(RTT::Logger::Debug);
  
  return RUN_ALL_TESTS();
}

