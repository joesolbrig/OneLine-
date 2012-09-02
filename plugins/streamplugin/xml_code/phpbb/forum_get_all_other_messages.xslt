<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="yes" omit-xml-declaration = "yes"
  encoding="iso-8859-1"/>

<xsl:template match="a[starts-with(@href,'viewtopic.php') and
                       contains(@href, '&amp;start=') and
                       not(contains(@href, 'watch=topic'))]">
  <next>
    <xsl:value-of select="substring-after(@href,'start=')"/>
  </next>
</xsl:template>

<xsl:template match="text()">
</xsl:template>

<xsl:template match="/">
  <nexts>
    <xsl:apply-templates/>
  </nexts>
</xsl:template>
</xsl:stylesheet>
