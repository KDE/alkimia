<?php

/*
 * Copyright 2020-2024 Ralf Habacker <ralf.habacker@freenet.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

 /*
  * online quotes test web service for alkimia
  */

function dump_single_data($a, $type, $dtype = "")
{
    $date = date("H:i:s(CE\S\T) d/m/Y");
    $value = floatval(rand(300, 600)) / 10;
    if ($type == "json") {
?>
{"chart":{"result":[{"meta":{"currency":"USD","symbol":"<?php echo $a ?>","exchangeName":"NYQ","instrumentType":"EQUITY","firstTradeDate":511021800,"regularMarketTime":1599746616,"gmtoffset":-14400,"timezone":"EDT","exchangeTimezoneName":"America/New_York","regularMarketPrice":<?php echo $value ?>,"chartPreviousClose":56.95,"previousClose":56.95,"scale":3,"priceHint":2,"currentTradingPeriod":{"pre":{"timezone":"EDT","start":1599724800,"end":1599744600,"gmtoffset":-14400},"regular":{"timezone":"EDT","start":1599744600,"end":1599768000,"gmtoffset":-14400},"post":{"timezone":"EDT","start":1599768000,"end":1599782400,"gmtoffset":-14400}},"tradingPeriods":[[{"timezone":"EDT","start":1599744600,"end":1599768000,"gmtoffset":-14400}]],"dataGranularity":"2m","range":"1d","validRanges":["1d","5d","1mo","3mo","6mo","1y","2y","5y","10y","ytd","max"]},"timestamp":[1599744600,1599744720,1599744840,1599744960,1599745080,1599745200,1599745320,1599745440,1599745560,1599745680,1599745800,1599745920,1599746040,1599746160,1599746280,1599746400,1599746520,1599746616],"indicators":{"quote":[{"close":[57.5099983215332,57.83000183105469,57.79999923706055,57.90999984741211,58.150001525878906,57.97999954223633,57.86000061035156,57.81999969482422,57.84000015258789,57.84000015258789,57.810001373291016,57.79999923706055,57.95000076293945,57.90660095214844,57.849998474121094,57.91999816894531,57.880001068115234,57.939998626708984],"low":[57.34000015258789,57.459999084472656,57.77000045776367,57.79999923706055,57.880001068115234,57.869998931884766,57.79999923706055,57.81999969482422,57.79999923706055,57.79999923706055,57.77000045776367,57.790000915527344,57.790000915527344,57.900001525878906,57.790000915527344,57.79999923706055,57.869998931884766,57.939998626708984],"open":[57.439998626708984,57.47999954223633,57.81570053100586,57.81999969482422,57.919898986816406,58.150001525878906,57.9900016784668,57.86000061035156,57.810001373291016,57.84000015258789,57.849998474121094,57.810001373291016,57.79499816894531,57.939998626708984,57.900001525878906,57.85499954223633,57.91999816894531,57.939998626708984],"high":[57.5099983215332,57.83000183105469,57.959999084472656,57.95000076293945,58.150001525878906,58.18000030517578,57.9900016784668,57.95000076293945,57.869998931884766,57.922401428222656,57.85499954223633,57.86000061035156,58.029998779296875,58.0099983215332,57.900001525878906,57.93000030517578,57.91999816894531,57.939998626708984],"volume":[243373,185489,134863,142816,156024,112022,130817,145509,75350,51761,104198,170484,139675,70435,49514,78868,30121,0]}]}}],"error":null}}
<?php
    } else if ($type == "csv") {
        echo "symbol;value;date\n";
        echo "$a;$value;$date\n";
    } else {
        if ($dtype == "javascript") {
            echo <<<EOD
<script type="text/javascript">
document.body.onload = addQuote("$value", "$date");

function addQuote(value, date) {
  const newDiv = document.createElement("span");
  document.createTextNode(value);
  newDiv.appendChild(document.createTextNode(value));
  newDiv.appendChild(document.createTextNode(" updated "));
  newDiv.appendChild(document.createTextNode(date));
  const currentDiv = document.getElementById("div");
  document.body.insertBefore(newDiv, currentDiv);
}
</script>
<div/>
EOD;
        } else {
            echo "$value $date</br>";
        }
    }
}

function dump_double_data($a, $b, $type)
{
    $date = date("H:i:s(CE\S\T) d/m/Y");
    $value = rand(7900, 8100);
    if ($type == "json") {
        echo '{"error": "not implemented yet"}';
    } else if ($type == "csv") {
        echo "symbol1;symbol2;rate;date;notes\n";
        echo "$a;$b;$value;$date\n";
    } else { ?>
<h1 style="margin-top:5px;color:#ec1b1b;"><span style="font-size:15px;color:#000;">1 Bitcoin =</span><br /> <?php echo $value ?> British Pound</h1>
<span class="datetime" style="display:inline-block; margin-top:10px; text-align:right; align:right; font-size:12px; color:#9c9c9c">updated <?php echo $date ?></span>
<?php }
}

# show errors
$debug = isset($_REQUEST['debug']);
if ($debug) {
    error_reporting(E_ALL);
    ini_set('display_errors', '1');
}

$isPost = $_SERVER['REQUEST_METHOD'] == "POST";

$self = ($_SERVER['SERVER_PORT'] == '443' ? 'https://' : 'http://').$_SERVER['SERVER_NAME'].$_SERVER['SCRIPT_NAME'];

# detect number of parameters
$params = $_REQUEST;

# return server error
$servererror= isset($params['servererror']) ? strtolower($params['servererror']) : "";
if ($servererror == "1") {
    http_response_code(500);
    exit(0);
}

$redirect= isset($params['redirect']) ? strtolower($params['redirect']) : "";
if ($redirect == "1") {
    unset($params['redirect']);
    $url = $self.'?'.http_build_query($params);
    header("Location: ".$url);
    http_response_code(301);
    exit(0);
}

$timeout= isset($params['timeout']) ? strtolower($params['timeout']) : "";
if ($timeout != "") {
    sleep(intval($timeout));
    header("Content-Type: text/plain; charset=utf-8");
    http_response_code(504);
    exit(0);
}

$type = isset($params['type']) ? strtolower($params['type']) : "html";
$dtype = isset($params['dtype']) ? strtolower($params['dtype']) : "";

$file_date = date("Y-m-d-H-i-s");
if ($type == "json") {
    header("Content-Type: application/json");
    header("Content-Disposition: inline; filename=\"data-$file_date.$type\"");
} else if ($type == "csv") {
    header("Content-Type: text/csv");
    header("Content-Disposition: inline; filename=\"data-$file_date.$type\"");
} else {
    header("Content-Type: text/html; charset=utf-8");
    echo "<html><body>";
}

# default output
if (count($params) == 0) {
?>
<p>This service is intended for access by KMyMoney, the online quote editor or any other application using the online quote support provided by alkimia.</p>
<p>To use this service, enter one of the following URL's in the URL field of the quote details widget:
<ul>
<li> with one parameter as html output:<pre><?php echo $self ?>?a=%1</pre></li>
<li> with one parameter as json output:<pre><?php echo $self ?>?a=%1&type=json</pre></li>
<li> with one parameter as csv output:<pre><?php echo $self ?>?a=%1&type=csv</pre></li>
<li> with two parameters as html output:<pre><?php echo $self ?>?a=%1&b=%2</pre></li>
<li> with two parameters as json output:<pre><?php echo $self ?>?a=%1&b=%2&type=json</pre></li>
<li> with two parameters as csv output:<pre><?php echo $self ?>?a=%1&b=%2&type=json</pre></li>
<li> with one parameter as html output generated by javascript:<pre><?php echo $self ?>?a=%1&dtype=javascript</pre></li>
</ul>
<?php
}

# handle requests
if (isset($params['a']) && isset($params['b'])) {
    dump_double_data($params['a'], $params['b'], $type);
} else if (isset($params['a'])) {
    dump_single_data($params['a'], $type, $dtype);
}

# debug output
if ($debug)
    echo "<p>Debug Information</p>"
        ."<pre>".print_r($GLOBALS, true)."</pre>\n";

if ($type == "html")
    echo "</body></html>";
