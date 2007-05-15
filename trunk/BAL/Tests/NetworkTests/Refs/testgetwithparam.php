<HTML>
<HEAD>
</HEAD>
<?
if( $HTTP_GET_VARS["param1"] == "toto" && $HTTP_GET_VARS["param2"] == "kiki" && count($HTTP_GET_VARS) == 2 )
{
  echo "TEST SUCCESSFULL";
}
else
{
  echo "TEST FAILED";
}
?>
</HTML>
