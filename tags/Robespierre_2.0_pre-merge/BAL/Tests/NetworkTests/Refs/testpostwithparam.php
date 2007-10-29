<HTML>
<HEAD>
</HEAD>
<?
echo "VARS ".$HTTP_POST_VARS["param1"]. " ".$HTTP_POST_VARS["param2"]." ".count($HTTP_POST_VARS) ;
if( $HTTP_POST_VARS["param1"] == "toto" && $HTTP_POST_VARS["param2"] == "kiki" && count($HTTP_POST_VARS) == 2 )
{
  echo "TEST SUCCESSFULL";
}
else
{
  echo "TEST FAILED";
}
?>
</HTML>
