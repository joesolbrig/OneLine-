<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="yes" omit-xml-declaration = "yes"
  encoding="iso-8859-1"/>

<xsl:template match="a[contains(@href,'login.php') and
                       contains(@href, 'logout')]">
  <logout>
    <xsl:text>/forum/fr/</xsl:text>
    <xsl:value-of select="@href"/>
  </logout>
</xsl:template>

<xsl:template match="text()">
</xsl:template>

<xsl:template match="/">
  <logouts>
    <xsl:apply-templates/>
  </logouts>
</xsl:template>

</xsl:stylesheet>
