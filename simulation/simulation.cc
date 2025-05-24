
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"
#include "ns3/ipv4-address-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HighFlowAnomalyDetection");

int main(int argc, char *argv[])
{
    Ipv4AddressGenerator::Reset();

    Time::SetResolution(Time::NS);
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    NodeContainer subnetA_normal, subnetB_normal, subnetC_normal,subnetD_normal,subnetE_normal;
    subnetA_normal.Create(100);
    subnetB_normal.Create(100);
    subnetC_normal.Create(100);
    subnetD_normal.Create(100);
    subnetE_normal.Create(100);



    Ptr<Node> attackerA = CreateObject<Node>();
    Ptr<Node> attackerB = CreateObject<Node>();
    Ptr<Node> attackerC = CreateObject<Node>();
    Ptr<Node> attackerD = CreateObject<Node>();
    Ptr<Node> attackerE = CreateObject<Node>();

    Ptr<Node> routerA = CreateObject<Node>();
    Ptr<Node> routerB = CreateObject<Node>();
    Ptr<Node> routerC = CreateObject<Node>();
    Ptr<Node> routerD = CreateObject<Node>();
    Ptr<Node> routerE = CreateObject<Node>();
    Ptr<Node> coreRouter = CreateObject<Node>();
    Ptr<Node> server = CreateObject<Node>();

    PointToPointHelper p2p, backbone;

    p2p.SetDeviceAttribute("DataRate", StringValue("1Mbps"));         // Düşük hız
    p2p.SetChannelAttribute("Delay", StringValue("20ms"));            // Yüksek gecikme
    p2p.SetQueue("ns3::DropTailQueue<Packet>","MaxSize",StringValue("1p"));
    backbone.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    backbone.SetChannelAttribute("Delay", StringValue("10ms"));
    backbone.SetQueue("ns3::DropTailQueue<Packet>","MaxSize",StringValue("1p"));
    // Bağlantılar
    std::vector<NetDeviceContainer> devA, devB, devC, devD, devE;
    for (uint32_t i = 0; i < subnetA_normal.GetN(); ++i)
        devA.push_back(p2p.Install(NodeContainer(subnetA_normal.Get(i), routerA)));
    NetDeviceContainer attackerA_dev = p2p.Install(NodeContainer(attackerA, routerA));

    for (uint32_t i = 0; i < subnetB_normal.GetN(); ++i)
        devB.push_back(p2p.Install(NodeContainer(subnetB_normal.Get(i), routerB)));
    NetDeviceContainer attackerB_dev = p2p.Install(NodeContainer(attackerB, routerB));

    for (uint32_t i = 0; i < subnetC_normal.GetN(); ++i)
        devC.push_back(p2p.Install(NodeContainer(subnetC_normal.Get(i), routerC)));

    NetDeviceContainer attackerC_dev = p2p.Install(NodeContainer(attackerC, routerC));

    for (uint32_t i = 0; i<subnetD_normal.GetN(); ++i)
        devD.push_back(p2p.Install(NodeContainer(subnetD_normal.Get(i),routerD)));
    
    NetDeviceContainer attackerD_dev = p2p.Install(NodeContainer(attackerD,routerD));

    for (uint32_t i = 0; i<subnetE_normal.GetN(); ++i)
        devE.push_back(p2p.Install(NodeContainer(subnetE_normal.Get(i),routerE)));

    NetDeviceContainer attackerE_dev = p2p.Install(NodeContainer(attackerE,routerE));
    
    NetDeviceContainer linkRA = backbone.Install(NodeContainer(routerA, coreRouter));
    NetDeviceContainer linkRB = backbone.Install(NodeContainer(routerB, coreRouter));
    NetDeviceContainer linkRC = backbone.Install(NodeContainer(routerC, coreRouter));
    NetDeviceContainer linkRD = backbone.Install(NodeContainer(routerD, coreRouter));
    NetDeviceContainer linkRE = backbone.Install(NodeContainer(routerE, coreRouter));
    NetDeviceContainer linkSrv = backbone.Install(NodeContainer(coreRouter, server));

    // Stack kurulumu
    InternetStackHelper stack;
    stack.InstallAll();

    // IP Atamaları
    Ipv4AddressHelper address;
    std::vector<Ipv4InterfaceContainer> ifaceA, ifaceB, ifaceC, ifaceD, ifaceE;
    char subnetBuf[20];

    for (size_t i = 0; i < subnetA_normal.GetN(); ++i) {
        sprintf(subnetBuf, "10.1.%lu.0", i + 1);
        address.SetBase(subnetBuf, "255.255.255.0");
        ifaceA.push_back(address.Assign(devA[i]));
    }
    address.SetBase("10.1.200.0", "255.255.255.0");
    address.Assign(attackerA_dev);

    for (size_t i = 0; i < subnetB_normal.GetN(); ++i) {
        sprintf(subnetBuf, "10.2.%lu.0", i + 1);
        address.SetBase(subnetBuf, "255.255.255.0");
        ifaceB.push_back(address.Assign(devB[i]));
    }
    address.SetBase("10.2.200.0", "255.255.255.0");
    address.Assign(attackerB_dev);

    for (size_t i = 0; i < subnetC_normal.GetN(); ++i) {
        sprintf(subnetBuf, "10.3.%lu.0", i + 1);
        address.SetBase(subnetBuf, "255.255.255.0");
        ifaceC.push_back(address.Assign(devC[i]));
    }
    address.SetBase("10.3.200.0", "255.255.255.0");
    address.Assign(attackerC_dev);

    for (size_t i = 0; i< subnetD_normal.GetN(); ++i){
        sprintf(subnetBuf, "10.4.%lu.0",i+1);
        address.SetBase(subnetBuf,"255.255.255.0");
        ifaceD.push_back(address.Assign(devD[i]));
    }

    address.SetBase("10.4.200.0","255.255.255.0");
    address.Assign(attackerD_dev);

    for (size_t i = 0; i< subnetE_normal.GetN(); ++i){
        sprintf(subnetBuf, "10.5.%lu.0",i+1);
        address.SetBase(subnetBuf,"255.255.255.0");
        ifaceE.push_back(address.Assign(devE[i]));
    }

    address.SetBase("10.5.200.0","255.255.255.0");
    address.Assign(attackerE_dev);

    address.SetBase("10.10.1.0", "255.255.255.0");
    Ipv4InterfaceContainer ifaceRA = address.Assign(linkRA);
    address.SetBase("10.10.2.0", "255.255.255.0");
    Ipv4InterfaceContainer ifaceRB = address.Assign(linkRB);
    address.SetBase("10.10.3.0", "255.255.255.0");
    Ipv4InterfaceContainer ifaceRC = address.Assign(linkRC);
    address.SetBase("10.10.4.0", "255.255.255.0");
    Ipv4InterfaceContainer ifaceRD = address.Assign(linkRD);
    address.SetBase("10.10.5.0","255.255.255.0");
    Ipv4InterfaceContainer ifaceRE = address.Assign(linkRE);
    

    Ipv4InterfaceContainer ifaceSrv = address.Assign(linkSrv);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Sunucuda birden fazla port dinle
    uint16_t baseTcpPort = 8000, baseUdpPort = 9000;
    for (uint16_t i = 0; i < 10; ++i) {
        PacketSinkHelper tcpSink("ns3::TcpSocketFactory", InetSocketAddress(ifaceSrv.GetAddress(1), baseTcpPort + i));
        tcpSink.Install(server);

        PacketSinkHelper udpSink("ns3::UdpSocketFactory", InetSocketAddress(ifaceSrv.GetAddress(1), baseUdpPort + i));
        udpSink.Install(server);
    }

    // Normal trafik: Her node 3 TCP bağlantısı kurar
    for (uint32_t i = 0; i < subnetA_normal.GetN(); ++i) {
        for (uint16_t j = 0; j < 10; ++j) {
            BulkSendHelper bulk("ns3::TcpSocketFactory", InetSocketAddress(ifaceSrv.GetAddress(1), baseTcpPort + j));
            bulk.SetAttribute("MaxBytes", UintegerValue(2048));
            ApplicationContainer app = bulk.Install(subnetA_normal.Get(i));
            app.Start(Seconds(1.0 + i * 0.1 + j));
            app.Stop(Seconds(250.0));
        }
    }

    for (uint16_t i = 0; i<subnetB_normal.GetN(); ++i){
        for (uint16_t j = 0; j<10; ++j){
            BulkSendHelper bulk("ns3::TcpSocketFactory",InetSocketAddress(ifaceSrv.GetAddress(1), baseTcpPort+j));
            bulk.SetAttribute("MaxBytes", UintegerValue(2048));
            ApplicationContainer app = bulk.Install(subnetB_normal.Get(i));
            app.Start(Seconds(1.0 + i * 0.1 + j));
            app.Stop(Seconds(250.0));
        }
    }

    for (uint16_t i = 0; i<subnetC_normal.GetN(); ++i){
        for (uint16_t j = 0; j<10; ++j){
            BulkSendHelper bulk("ns3::TcpSocketFactory",InetSocketAddress(ifaceSrv.GetAddress(1), baseTcpPort+j));
            bulk.SetAttribute("MaxBytes", UintegerValue(2048));
            ApplicationContainer app = bulk.Install(subnetC_normal.Get(i));
            app.Start(Seconds(1.0 + i * 0.1 + j));
            app.Stop(Seconds(250.0));
        }
    }

    for (uint16_t i = 0; i<subnetD_normal.GetN(); ++i){
        for (uint16_t j = 0; j<10; ++j){
            BulkSendHelper bulk("ns3::TcpSocketFactory",InetSocketAddress(ifaceSrv.GetAddress(1), baseTcpPort+j));
            bulk.SetAttribute("MaxBytes", UintegerValue(2048));
            ApplicationContainer app = bulk.Install(subnetD_normal.Get(i));
            app.Start(Seconds(1.0 + i * 0.1 + j));
            app.Stop(Seconds(250.0));
        }
    }
    for (uint16_t i = 0; i<subnetE_normal.GetN(); ++i){
        for (uint16_t j = 0; j<10; ++j){
            BulkSendHelper bulk("ns3::TcpSocketFactory",InetSocketAddress(ifaceSrv.GetAddress(1), baseTcpPort+j));
            bulk.SetAttribute("MaxBytes", UintegerValue(2048));
            ApplicationContainer app = bulk.Install(subnetE_normal.Get(i));
            app.Start(Seconds(1.0 + i * 0.1 + j));
            app.Stop(Seconds(250.0));
        }
    }

    // 🔵 Bu saldırı normal trafiğe benzeyecek şekilde yavaşlatıldı.
    for (uint16_t port = 9000; port < 9010; port++) {
        UdpClientHelper stealthFlood(ifaceSrv.GetAddress(1), port);
        stealthFlood.SetAttribute("MaxPackets", UintegerValue(1000));
        stealthFlood.SetAttribute("Interval", TimeValue(MilliSeconds(200)));  // Daha düşük hız
        stealthFlood.SetAttribute("PacketSize", UintegerValue(512));
        stealthFlood.Install(attackerA).Start(Seconds(10.0 + port * 0.5));
    }

    // Port scan (100 UDP porta)
    for (uint16_t port = baseUdpPort + 1; port <= baseUdpPort + 100; port++) {
        UdpClientHelper scanner(ifaceSrv.GetAddress(1), port);
        scanner.SetAttribute("MaxPackets", UintegerValue(1));
        scanner.SetAttribute("Interval", TimeValue(Seconds(0.2)));
        scanner.SetAttribute("PacketSize", UintegerValue(128));
        scanner.Install(attackerB).Start(Seconds(20.0 + port * 0.01));
    }

    // Brute force TCP (çoklu kısa oturumlar)
    for (uint32_t i = 0; i < 100; ++i) {
        BulkSendHelper brute("ns3::TcpSocketFactory", InetSocketAddress(ifaceSrv.GetAddress(1), baseTcpPort));
        brute.SetAttribute("MaxBytes", UintegerValue(1024));
        ApplicationContainer app = brute.Install(attackerC);
        app.Start(Seconds(30.0 + i * 0.5));
        app.Stop(Seconds(60.0));
    }

    // ICMP 
    // Attacker D - ICMP Ping Flood
    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    Ptr<Socket> icmpFloodSocket = Socket::CreateSocket(attackerD, tid);
    InetSocketAddress remote = InetSocketAddress(ifaceSrv.GetAddress(1), 9);
    icmpFloodSocket->Connect(remote);

    Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
    for (uint32_t i = 0; i < 1000; ++i)
    {
        Simulator::Schedule(Seconds(40.0 + i * 0.01), [&]() {
            Ptr<Packet> packet = Create<Packet>(64);  // Küçük boyutlu ping gibi
            icmpFloodSocket->Send(packet);
        });
    }

    // Connection reset flow
    for (uint16_t port = baseTcpPort; port < baseTcpPort + 10; ++port)
{
    for (uint32_t i = 0; i < 100; ++i)
    {
        BulkSendHelper shortConn("ns3::TcpSocketFactory", InetSocketAddress(ifaceSrv.GetAddress(1), port));
        shortConn.SetAttribute("MaxBytes", UintegerValue(128));  // Çok kısa veri
        ApplicationContainer app = shortConn.Install(attackerE);
        app.Start(Seconds(50.0 + i * 0.05));
        app.Stop(Seconds(50.1 + i * 0.05));  // 0.1 saniyelik bağlantılar
    }
}

    // FlowMonitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream("trace.tr");
    p2p.EnableAsciiAll(stream);
    backbone.EnableAsciiAll(stream);
    Simulator::Stop(Seconds(600.0));
    Simulator::Run();
    monitor->SerializeToXmlFile("//home//vboxuser//Desktop//ns-allinone-3.36.1//ns-3.36.1//scratch//flowmon_results.xml", true, true);
    monitor->CheckForLostPackets();


    

    // 🔵 NetAnim için animasyon objesi
    AnimationInterface anim("network-topology.xml");

// 📍 Router ve server pozisyonları
anim.SetConstantPosition(routerA, 10, 30);
anim.SetConstantPosition(routerB, 10, 60);
anim.SetConstantPosition(routerC, 10, 90);
anim.SetConstantPosition(routerD, 10, 120);
anim.SetConstantPosition(routerE, 10, 150);
anim.SetConstantPosition(coreRouter, 50, 90);
anim.SetConstantPosition(server, 90, 90);

// 👥 Subnet A (sadece ilk 30 kullanıcıyı çiz)
for (uint32_t i = 0; i < std::min((uint32_t)30, subnetA_normal.GetN()); ++i)
    anim.SetConstantPosition(subnetA_normal.Get(i), 0, 30 + i * 0.5);
anim.SetConstantPosition(attackerA, 0, 20);

// 👥 Subnet B
for (uint32_t i = 0; i < std::min((uint32_t)30, subnetB_normal.GetN()); ++i)
    anim.SetConstantPosition(subnetB_normal.Get(i), 0, 60 + i * 0.5);
anim.SetConstantPosition(attackerB, 0, 50);

// 👥 Subnet C
for (uint32_t i = 0; i < std::min((uint32_t)30, subnetC_normal.GetN()); ++i)
    anim.SetConstantPosition(subnetC_normal.Get(i), 0, 90 + i * 0.5);
anim.SetConstantPosition(attackerC, 0, 80);

// 👥 Subnet D
for (uint32_t i = 0; i < std::min((uint32_t)30, subnetD_normal.GetN()); ++i)
    anim.SetConstantPosition(subnetD_normal.Get(i), 0, 120 + i * 0.5);
anim.SetConstantPosition(attackerD, 0, 110);

// 👥 Subnet E
for (uint32_t i = 0; i < std::min((uint32_t)30, subnetE_normal.GetN()); ++i)
    anim.SetConstantPosition(subnetE_normal.Get(i), 0, 150 + i * 0.5);
anim.SetConstantPosition(attackerE, 0, 140);

    Simulator::Stop(Seconds(600.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}