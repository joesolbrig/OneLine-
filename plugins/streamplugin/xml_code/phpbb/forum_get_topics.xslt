<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="yes" omit-xml-declaration = "yes"
  encoding="iso-8859-1"/>

<xsl:template match="tr[count(*) = 6]">
<topic>
  <title>
    <xsl:value-of select="td[2]/span/a[position()=last()]"/>
  </title>
  <date>
    <xsl:value-of select="td[6]/span/text()"/>
  </date>
  <author>
    <xsl:value-of select="td[6]/span/a[1]/text()"/>
  </author>
  <link>
    <xsl:text>/forum/fr/</xsl:text>
    <xsl:value-of select="td[2]/span/a[position()=last()]/@href"/>
  </link>
</topic>
</xsl:template>

<xsl:template match="/">
  <topics>
    <xsl:apply-templates/>
  </topics>
</xsl:template>

<xsl:template match="text()">
</xsl:template>

</xsl:stylesheet>
