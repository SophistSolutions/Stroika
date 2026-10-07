SSDPServer shows how to use the Stroika UPnP framework to make a UPnP device: a light, switched on and off over the network -
the UPnP Forum's standard BinaryLight device, with its SwitchPower service. SSDP (UPnP::SSDP::Server::BasicServer) advertises
it, and the Stroika WebServer framework serves its description, its service's description, and its service's actions (SOAP).

To switch it on - from this machine or another - run the SSDPClient sample:
    SSDPClient -s urn:schemas-upnp-org:service:SwitchPower:1 --switch on
or send the SOAP request yourself, to the address its LOCATION gives (e.g. http://192.168.1.5:8080/):
    curl http://192.168.1.5:8080/SwitchPower/control -H 'Content-Type: text/xml; charset="utf-8"' \
        -H 'SOAPACTION: "urn:schemas-upnp-org:service:SwitchPower:1#SetTarget"' \
        -d '<?xml version="1.0"?><s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><s:Body><u:SetTarget xmlns:u="urn:schemas-upnp-org:service:SwitchPower:1"><newTargetValue>1</newTargetValue></u:SetTarget></s:Body></s:Envelope>'

Not shown: eventing (GENA), telling subscribers each change of the light's Status. For a web service of your own design,
rather than a standard UPnP one, see the WebService sample.
