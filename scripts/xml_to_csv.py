import xml.etree.ElementTree as ET
import pandas as pd

def parse_time_ns_to_seconds(time_str):
    try:
        return float(time_str.lower().replace("ns", "")) / 1e9
    except:
        return 0.0

def parse_flowmonitor_xml(xml_path):
    tree = ET.parse(xml_path)
    root = tree.getroot()

    data = []

    flow_meta = {}
    for flow in root.find("Ipv4FlowClassifier").findall("Flow"):
        fid = int(flow.attrib["flowId"])
        flow_meta[fid] = {
            "source": flow.attrib["sourceAddress"],
            "destination": flow.attrib["destinationAddress"],
            "protocol": int(flow.attrib["protocol"]),
            "sourcePort": int(flow.attrib["sourcePort"]),
            "destinationPort": int(flow.attrib["destinationPort"])
        }

    for flow in root.find("FlowStats").findall("Flow"):
        fid = int(flow.attrib["flowId"])
        first_tx = parse_time_ns_to_seconds(flow.attrib.get("timeFirstTxPacket", "0"))
        last_rx = parse_time_ns_to_seconds(flow.attrib.get("timeLastRxPacket", "0"))
        duration = last_rx - first_tx

        stats = {
            "flowId": fid,
            "txBytes": int(flow.attrib.get("txBytes", 0)),
            "rxBytes": int(flow.attrib.get("rxBytes", 0)),
            "txPackets": int(flow.attrib.get("txPackets", 0)),
            "rxPackets": int(flow.attrib.get("rxPackets", 0)),
            "lostPackets": int(flow.attrib.get("lostPackets", 0)),
            "delaySum_ns": float(flow.attrib.get("delaySum", "0").replace("+", "").replace("ns", "")),
            "jitterSum_ns": float(flow.attrib.get("jitterSum", "0").replace("+", "").replace("ns", "")),
            "timeFirstTxPacket": first_tx,
            "timeLastRxPacket": last_rx,
            "throughput_kbps": (float(flow.attrib.get("rxBytes", 0)) * 8 / (duration * 1000)) if duration > 0 else 0.0
        }

        if fid in flow_meta:
            stats.update(flow_meta[fid])

        data.append(stats)

    return pd.DataFrame(data)

# Kullanım:
df_flows = parse_flowmonitor_xml("flowmon_results.xml")
df_flows.to_csv("df_flows.csv", index=False)
print(df_flows.head())
