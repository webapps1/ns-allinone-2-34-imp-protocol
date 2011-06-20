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
set val(nn)             2                          ;# number of mobilenodes
set val(rp)             XFXVanets                       ;# routing protocol
set val(x)              500   			   ;# X dimension of topography
set val(y)              400   			   ;# Y dimension of topography
set val(stop)		100			   ;# time of simulation end

set ns		  [new Simulator]
set tracefd       [open simple.tr w]
set windowVsTime2 [open win.tr w]
set namtrace      [open simwrls.nam w]

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

# Rota
$ns at 1.0 "$node_(0) route 5.0 5.0 0.0 250.0 250.0 0.0"

# Provide initial location of mobilenodes

# Configurado como um device movel
$node_(0) set X_ 5.0
$node_(0) set Y_ 5.0
$node_(0) set Z_ 0.0
$node_(0) set kind 2

# Configurado como device estático
$node_(1) set X_ 280.0
$node_(1) set Y_ 180.0
$node_(1) set Z_ 0.0
$node_(1) set kind 1

# Generation of movements
$ns at 10.0 "$node_(0) setdest 250.0 250.0 10.0"

#---------------------- configure node 0 and node 1
#---------------------- node 0 as tcp and node 1 as sink
set tcp01 [new Agent/TCP/Newreno]
$ns attach-agent $node_(0) $tcp01

set sink01 [new Agent/TCPSink]
$ns attach-agent $node_(1) $sink01

$ns connect $tcp01 $sink01

set ftp01 [new Application/FTP]
$ftp01 attach-agent $tcp01
$ns at 1.0 "$ftp01 start"

# Printing the window size

# ending nam and the simulation
$ns at $val(stop) "$ns nam-end-wireless $val(stop)"
$ns at $val(stop) "stop"
$ns at 100.0 "puts \"end simulation\" ; $ns halt"

proc stop {} {
    global ns tracefd namtrace
    $ns flush-trace
    close $tracefd
    close $namtrace
    #Execute nam on the trace file
    exec nam simwrls.nam &
    exit 0
}

#Call the finish procedure after 5 seconds of simulation time
$ns run 
