# wrls1.tcl
# A 3-node example for ad-hoc simulation with AODV

# Define options
set val(chan)           Channel/WirelessChannel    ;# channel type
set val(prop)           Propagation/TwoRayGround   ;# radio-propagation model
set val(netif)          Phy/WirelessPhy            ;# network interface type
set val(mac)            Mac/802_11                 ;# MAC type
set val(ifq)            Queue/DropTail/PriQueue    ;# interface queue type
set val(ll)             LL                         ;# link layer type
set val(ant)            Antenna/OmniAntenna        ;# antenna model
set val(ifqlen)         50                         ;# max packet in ifq
set val(nn)             7                          ;# number of mobilenodes
set val(rp)             XFXVanets                  ;# routing protocol
set val(x)              500   			   ;# X dimension of topography
set val(y)              400   			   ;# Y dimension of topography
set val(stop)		100			   ;# time of simulation end

set ns		  [new Simulator]
set tracefd       [open simple.tr w]
set windowVsTime2 [open win.tr w]
set namtrace      [open simwrls.nam w]

set dist(14m) 9.81011e-07
set dist(20m) 4.80696e-07
set dist(40m) 1.20174e-07
Phy/WirelessPhy set CSThresh_ $dist(14m)
Phy/WirelessPhy set RXThresh_ $dist(14m)

$ns trace-all $tracefd
$ns namtrace-all-wireless $namtrace $val(x) $val(y)

# set up topography object
set topo       [new Topography]

$topo load_flatgrid $val(x) $val(y)

create-god $val(nn)

#
#  Create nn mobilenodes [$val(nn)] and attach them to the channel.
#

# configure the nodes
$ns node-config -adhocRouting $val(rp) \
-llType $val(ll) \
-macType $val(mac) \
-ifqType $val(ifq) \
-ifqLen $val(ifqlen) \
-antType $val(ant) \
-propType $val(prop) \
-phyType $val(netif) \
-channelType $val(chan) \
-topoInstance $topo \
-agentTrace ON \
-routerTrace ON \
-macTrace OFF \
-movementTrace ON

for {set i 0} {$i < $val(nn) } { incr i } {
    set node_($i) [$ns node]
}

# 0, 1, 4 como devices.
# 2, 3, 5, 6, como carros.

# Rota
$ns at 0.0 "$node_(2) route 5.0 5.0 10.0 25.0 25.0 10.0 150.0 150.10 250.0 250.0 10.0"
$ns at 0.0 "$node_(3) route 250.0 250.0 10.0 150.0 150.0 10.0 180.0 180.0 10.0 5.0 5.0 10.0"

$ns at 0.0 "$node_(5) route 180.0 180.0 10.0 160.0 160.0 10.0 250.0 250.0 10.0 5.0 5.0 10.0"
$ns at 0.0 "$node_(6) route 5.0 5.0 10.0 150.0 150.0 10.0 250.0 250.0 10.0 150.0 150.0 10.0"

# Configuração dos devices
$node_(0) set X_ 250.0
$node_(0) set Y_ 250.0
$node_(0) set Z_ 10.0
$node_(0) set kind 1

$node_(1) set X_ 5.0
$node_(1) set Y_ 5.0
$node_(1) set Z_ 10.0
$node_(1) set kind 1

$node_(4) set X_ 150.0
$node_(4) set Y_ 150.0
$node_(4) set Z_ 10.0
$node_(4) set kind 1

# Configurado como um device movel
$node_(2) set X_ 5.0
$node_(2) set Y_ 5.0
$node_(2) set Z_ 10.0
$node_(2) set kind 2

$node_(3) set X_ 250.0
$node_(3) set Y_ 250.0
$node_(3) set Z_ 10.0
$node_(3) set kind 2

$node_(5) set X_ 180.0
$node_(5) set Y_ 180.0
$node_(5) set Z_ 10.0
$node_(5) set kind 1

$node_(6) set X_ 5.0
$node_(6) set Y_ 5.0
$node_(6) set Z_ 10.0
$node_(6) set kind 1

# Generation of movements
# devices
$ns at 1.0 "$node_(0) setdest 250.0 250.0 10.0"
$ns at 1.0 "$node_(1) setdest 5.0 5.0 10.0"
$ns at 1.0 "$node_(4) setdest 150.0 150.0 10.0"

# carros
$ns at 1.0 "$node_(2) setdest 5.0 5.0 10.0"
$ns at 1.0 "$node_(3) setdest 250.0 250.0 10.0"
$ns at 1.0 "$node_(5) setdest 180.0 180.0 10.0"
$ns at 1.0 "$node_(6) setdest 5.0 5.0 10.0"

$ns at 6.0 "$node_(2) setdest 25.0 25.0 10.0"
$ns at 6.0 "$node_(3) setdest 150.0 150.0 10.0"
$ns at 6.0 "$node_(5) setdest 160.0 160.0 10.0"
$ns at 6.0 "$node_(6) setdest 150.0 150.0 10.0"

$ns at 12.0 "$node_(2) setdest 150.0 150.0 10.0"
$ns at 12.0 "$node_(3) setdest 180.0 180.0 10.0"
$ns at 12.0 "$node_(5) setdest 250.0 250.0 10.0"
$ns at 12.0 "$node_(6) setdest 250.0 250.0 10.0"

$ns at 18.0 "$node_(2) setdest 250.0 250.0 10.0"
$ns at 18.0 "$node_(3) setdest 5.0 5.0 10.0"
$ns at 18.0 "$node_(5) setdest 5.0 5.0 10.0"
$ns at 18.0 "$node_(6) setdest 150.0 150.0 10.0"


#---------------------- 2-> 0 3->1 5-> 4
#---------------------- configure node 0 and node 1
#---------------------- node 0 as tcp and node 1 as sink
set udp0 [$ns create-connection UDP $node_(2) LossMonitor $node_(0) 0]
$udp0 set fid_ 1
set cbr0 [$udp0 attach-app Traffic/CBR]
$cbr0 set packetSize_ 1000   
$cbr0 set interval_ 1.0
$ns at 1.0 "$cbr0 start"
$ns at 98.0 "$cbr0 stop"

set udp1 [$ns create-connection UDP $node_(3) LossMonitor $node_(1) 0]
$udp1 set fid_ 1
set cbr1 [$udp1 attach-app Traffic/CBR]
$cbr1 set packetSize_ 1000   
$cbr1 set interval_ 1.0
$ns at 1.0 "$cbr1 start"
$ns at 98.0 "$cbr1 stop"

set udp2 [$ns create-connection UDP $node_(5) LossMonitor $node_(4) 0]
$udp2 set fid_ 1
set cbr2 [$udp2 attach-app Traffic/CBR]
$cbr2 set packetSize_ 1000   
$cbr2 set interval_ 1.0
$ns at 1.0 "$cbr2 start"
$ns at 98.0 "$cbr2 stop"


$ns at $val(stop) "$ns nam-end-wireless $val(stop)"
$ns at $val(stop) "stop"
$ns at 100.0 "puts \"end simulation\" ; $ns halt"

proc stop {} {
    global ns tracefd namtrace
    $ns flush-trace
    close $tracefd
    #close $namtrace
    #Execute nam on the trace file
    #exec nam simwrls.nam &
    exit 0
}

#Call the finish procedure after 5 seconds of simulation time
$ns run 
