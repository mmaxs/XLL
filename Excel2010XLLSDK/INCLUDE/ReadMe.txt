Changes to XLCALL.H in the Microsoft Excel 2010 XLL Software Development Kit (SDK)
==================================================================================

1. Added support for new Microsoft Excel 2010 worksheet functions.

2. Added and modified definitions to support 64-bit XLL development.
   See the Excel 2010 XLL SDK documentation for details (under the \doc folder).

3. Added support for asynchronous UDFs and for compute cluster function remoting.
   See the Excel 2010 XLL SDK documentation for details (under the \doc folder).

4. Added an error value - xlerrGettingData.
   This error value was introduced in Microsoft Excel 2007.
   It occurs while refreshing external data for cube worksheet function calls.


Notes for International Applications
====================================

The Excel 2010 XLL SDK does not include localized versions of XLCALL.H.
Instead, the INTLMAP.XLSX workbook which shows you how the localized
command and function names map to the English-language constants used
in calls to the Excel12, Excel12v, Excel4 and Excel4v functions.

If you have code written for early versions of Microsoft Excel using a
localized version of XLCALL.H, you can use one of the following strategies
to update the code for Microsoft Excel 2010:

1. If you do not need any of the added functionality exposed in the Excel 2010 XLL SDK,
you can continue to use the localized version of XLCALL.H that shipped with the
Excel 4.0 XLL SDK.

2. You can use the constants in the new Excel 2010 XLL SDK XLCALL.H by using the
international-mapping workbook to change localized constants in your application.
Once you have changed the constants, your code will work with future versions of XLCALL.H,
and you can use the commands and functions exposed in the Excel 2010 XLL SDK.

3. You can continue to use a localized version of XLCALL.H and use the
international-mapping workbook to determine which new constant definitions
your code requires. You can add these new definitions to your application
and use the new features. However, for increased maintainability it is recommended
that you update your code to use the Excel 2010 XLL SDK constants.
