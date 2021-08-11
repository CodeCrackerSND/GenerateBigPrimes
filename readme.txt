Générateur de grands nombres premiers (plus de cent chiffres)-------------------------------------------------------------
Url     : http://codes-sources.commentcamarche.net/source/11353-generateur-de-grands-nombres-premiers-plus-de-cent-chiffresAuteur  : ymca2003Date    : 06/09/2013
Licence :
=========

Ce document intitulé « Générateur de grands nombres premiers (plus de cent chiffres) » issu de CommentCaMarche
(codes-sources.commentcamarche.net) est mis à disposition sous les termes de
la licence Creative Commons. Vous pouvez copier, modifier des copies de cette
source, dans les conditions fixées par la licence, tant que cette note
apparaît clairement.

Description :
=============

Ce code permet de g&eacute;n&eacute;rer de tr&egrave;s grands nombres premier (d
e 256 bits et plus). Le programme utilis&eacute; est un test probabiliste (le no
mbre est premier avec une certaine marge d'erreur). Cependant, en augmentant le 
nombre d'it&eacute;ration on peut avoir une marge d'erreur inf&eacute;rieure &ag
rave; 1/10^20.
<br />2 tests de primalit&eacute; sont propos&eacute;s : test de
 Solovay et Strassen ainsi que Miller-Rabin.
<br />Les nombres sont cod&eacute;
s en utilisant une classe qui permet de g&eacute;rer dynamiquement des nombres d
'une tr&egrave;s grande taille (aucune limite fix&eacute;e)
<br />Enfin, le pro
gramme peut g&eacute;n&eacute;rer les param&egrave;tres pour les cl&eacute;s de 
l'algorithme de cryptage RSA &agrave; partir des nombres premiers trouv&eacute;s
.
