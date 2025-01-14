#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("P2PThreeNodes");

int main() {
	LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
	LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);

	NodeContainer nodes;
	nodes.Create(3);

	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

	NetDeviceContainer devices01;
	devices01 = pointToPoint.Install(nodes.Get(0), nodes.Get(1));

	NetDeviceContainer devices02;
	devices02 = pointToPoint.Install(nodes.Get(0), nodes.Get(2));

	InternetStackHelper stack;
	stack.Install(nodes);

	Ipv4AddressHelper address01;
	address01.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces01 = address01.Assign(devices01);

	Ipv4AddressHelper address02;
	address02.SetBase("10.1.2.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces02 = address02.Assign(devices02);

	UdpEchoServerHelper echoServer(9);
	ApplicationContainer serverApps = echoServer.Install(nodes.Get(0));
	serverApps.Start(Seconds(1.0));
	serverApps.Stop(Seconds(10.0));

	UdpEchoClientHelper echoClient01(interfaces01.GetAddress(0), 9);
	echoClient01.SetAttribute("MaxPackets", UintegerValue(10));
	echoClient01.SetAttribute("PacketSize", UintegerValue(1024));
	echoClient01.SetAttribute("Interval", TimeValue(Seconds(1.0)));

	ApplicationContainer clientApps01 = echoClient01.Install(nodes.Get(1));
	clientApps01.Start(Seconds(2.0));
	clientApps01.Stop(Seconds(10.0));

	UdpEchoClientHelper echoClient02(interfaces02.GetAddress(0), 9);
	echoClient02.SetAttribute("MaxPackets", UintegerValue(10));
	echoClient02.SetAttribute("PacketSize", UintegerValue(1024));
	echoClient02.SetAttribute("Interval", TimeValue(Seconds(1.0)));

	ApplicationContainer clientApps02 = echoClient02.Install(nodes.Get(2));
	clientApps02.Start(Seconds(3.0));
	clientApps02.Stop(Seconds(10.0));

	AsciiTraceHelper ascii;
	pointToPoint.EnableAsciiAll(ascii.CreateFileStream("p2p-three-nodes-trace.tr"));

	AnimationInterface anim("ThreeNodesAnimation.xml");
	anim.SetConstantPosition(nodes.Get(0), 50, 50);
	anim.SetConstantPosition(nodes.Get(1), 0, 100);
	anim.SetConstantPosition(nodes.Get(2), 100, 100);

	anim.UpdateNodeDescription(nodes.Get(0), "Server");
	anim.UpdateNodeDescription(nodes.Get(1), "Client");
	anim.UpdateNodeDescription(nodes.Get(2), "Client");

	anim.UpdateNodeColor(nodes.Get(0), 0, 255, 0);

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}
