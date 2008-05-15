<?xml version="1.0" encoding="UTF-8"?>
<!--
// File "ISTtoTeX.xslt"
//
// blahtexml (version 0.5)
// Copyright (C) 2008, Gilles Van Assche
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>

<xsl:output method="text" indent="no" encoding="UTF-8"/>

<xsl:template match="symbols">
<xsl:text>\begin{longtable}{|l|l|l|}
\hline
Symbol &amp; Unicode &amp; Translated as \\
\endhead
\hline
</xsl:text>
    <xsl:apply-templates select="symbol"/>
<xsl:text>\end{longtable}
</xsl:text>
</xsl:template>

<xsl:template match="symbol">
    <xsl:text>$</xsl:text>
    <xsl:value-of select="@tex"/>
    <xsl:text>$ &amp; \texttt{</xsl:text>
    <xsl:value-of select="@unicode"/>
    <xsl:text>} &amp; \verb|</xsl:text>
    <xsl:value-of select="@tex"/>
    <xsl:text>| \\
\hline
</xsl:text>
</xsl:template>

</xsl:stylesheet>
