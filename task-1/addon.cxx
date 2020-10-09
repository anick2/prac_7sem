#include <addon.hxx>

#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/text/XTextTablesSupplier.hpp>
#include <com/sun/star/text/XTextTable.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <cppuhelper/supportsservice.hxx>
#include <rtl/ustring.hxx>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::frame;
using namespace com::sun::star::table;
using namespace com::sun::star::text;
using namespace com::sun::star::container;
using com::sun::star::beans::PropertyValue;
using com::sun::star::util::URL;
using rtl::OUString;

#define SERVICE_NAME "com.sun.star.frame.ProtocolHandler"



void TransposeTable(Reference <XTextTable> &xTable) {

    Reference < XCellRange > xCellRange(xTable, UNO_QUERY);
    Reference<XTextTableCursor> cur = xTable->createCursorByCellName(OUString::createFromAscii("A1"));

    cur->gotoEnd(false);
    int columns = cur->getRangeName()[0] - 'A' + 1;
    std::string tmp;
    for (int i = 1; i < cur->getRangeName().getLength(); i++){
        tmp += cur->getRangeName()[i];
    }
    
    int rows = std::atoi(tmp.c_str());

    int size = std::min(rows, columns);
    
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < i; j++)
        {
            Reference < XText > xTextij (xCellRange->getCellByPosition(j, i), UNO_QUERY);
            Reference < XText > xTextji (xCellRange->getCellByPosition(i, j), UNO_QUERY);
            rtl::OUString tmpString = xTextij->getString();
            xTextij->setString(xTextji->getString());
            xTextji->setString(tmpString);
        }
    }
}



void TransposeSubtables( Reference< XFrame > &rxFrame ) {

    if (!rxFrame.is())
        return;

    Reference < XController > xCtrl = rxFrame->getController();

    if (!xCtrl.is())
        return;

    Reference < XModel > xModel = xCtrl->getModel();

    if (!xModel.is())
        return;

    Reference < XTextDocument > xTextDocument (xModel, UNO_QUERY);

    if (!xTextDocument.is())
        return;


    Reference < XTextTablesSupplier > xTextTablesSupplier(xTextDocument, UNO_QUERY);
    Reference < XNameAccess > xTables = xTextTablesSupplier->getTextTables();


    for (auto& tname : xTables->getElementNames())
    {
        Any table = xTables->getByName(tname);
        Reference < XTextTable > xTable;
        table >>= xTable;

        TransposeTable(xTable);
    }
}

void GenerateTables(Reference < XFrame > &rxFrame) {   

    if (!rxFrame.is())
        return;

    Reference<XComponentLoader> rComponentLoader(rxFrame, UNO_QUERY);

    if ( !rComponentLoader.is() )
        return;

    Reference<XComponent> xWriterComponent = rComponentLoader->loadComponentFromURL(
        OUString::createFromAscii("private:factory/swriter"),
        OUString::createFromAscii("_blank"),
        0,
        Sequence<PropertyValue>());
    
    Reference<XTextDocument> xTextDocument (xWriterComponent, UNO_QUERY);

    Reference<XText> xText = xTextDocument->getText();
    Reference<XTextCursor> xTextCursor = xText->createTextCursor();

    int countTable = 2 + std::rand() % 7;
    for (int i = 0; i < countTable; i++) {

        Reference<XMultiServiceFactory> oDocMSF(xTextDocument, UNO_QUERY);
        Reference<XTextTable> xTable(oDocMSF->createInstance(OUString::createFromAscii("com.sun.star.text.TextTable")), UNO_QUERY);
 
        int rows = 3 + std::rand() % 8;
        int columns = 3 + std::rand() % 4;
        xTable->initialize(rows, columns);

        xTextCursor->gotoEnd(false);
        std::string tablesize( "  (" + std::to_string(rows) + "x" + std::to_string(columns) + ")");
        std::string s("Table " + std::to_string(i + 1) + tablesize);
        xTextCursor->setString(OUString::createFromAscii(s.c_str()));

        Reference<XTextContent> xTextContent(xTable, UNO_QUERY);
        xText->insertTextContent(xText->getEnd(), xTextContent, (unsigned char) 0);

        for (int i1 = 0; i1 < rows; i1++) {
            for (int i2 = 0; i2 < columns; i2++) {

                std::string cell((char)('A' + i2) + std::to_string(i1 + 1));
                Reference<XCell> xCell = xTable->getCellByName(OUString::createFromAscii(cell.c_str()));
                Reference<XText> xTextCell(xCell, UNO_QUERY);
                Reference<XTextCursor> xTextCursor2 = xTextCell->createTextCursor();

                std::string str("row_" + std::to_string(i1) + std::string(" col_") + std::to_string(i2));
                xTextCursor2->setString(OUString::createFromAscii(str.c_str()));
            }
        }
        xTextCursor->gotoEnd(false);
        xTextCursor->setString(OUString::createFromAscii("\n\n"));
    }

}


void SAL_CALL TableDispatchImpl::dispatch( const URL& aURL, const Sequence < PropertyValue >& lArgs ) throw (RuntimeException) {
    
    if ( aURL.Protocol.equalsAscii("inco.niocs.test.protocolhandler:") ) {
        if ( aURL.Path.equalsAscii("GenerateTables") ) {
            GenerateTables(mxFrame);
        }
        else if ( aURL.Path.equalsAscii("TransposeSubtables") ) {
	        TransposeSubtables(mxFrame);
        }
    }
}


void SAL_CALL TableDispatchImpl::addStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL ) throw (RuntimeException)
{
}

void SAL_CALL TableDispatchImpl::removeStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL ) throw (RuntimeException)
{
}


void SAL_CALL Addon::initialize( const Sequence< Any >& aArguments ) throw ( Exception, RuntimeException) {
    Reference < XFrame > xFrame;
    if ( aArguments.getLength() )
    {
        aArguments[0] >>= xFrame;
        mxFrame = xFrame;
    }
}


Reference< XDispatch > SAL_CALL Addon::queryDispatch( const URL& aURL, const ::rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags ) throw( RuntimeException ) {
    Reference < XDispatch > xRet;
    if ( aURL.Protocol.equalsAscii("inco.niocs.test.protocolhandler:") )
    {
        if ( aURL.Path.equalsAscii( "GenerateTables" ) || aURL.Path.equalsAscii( "TransposeSubtables" ) )
            xRet = new TableDispatchImpl( mxFrame );
    }

    return xRet;
}


Sequence < Reference< XDispatch > > SAL_CALL Addon::queryDispatches( const Sequence < DispatchDescriptor >& seqDescripts ) throw( RuntimeException ) {
    sal_Int32 nCount = seqDescripts.getLength();
    Sequence < Reference < XDispatch > > lDispatcher( nCount );

    for( sal_Int32 i = 0; i < nCount; ++i )
        lDispatcher[i] = queryDispatch( seqDescripts[i].FeatureURL, seqDescripts[i].FrameName, seqDescripts[i].SearchFlags );

    return lDispatcher;
}


OUString Addon_getImplementationName() throw (RuntimeException) {
    return OUString ( IMPLEMENTATION_NAME );
}


Sequence< ::rtl::OUString > SAL_CALL Addon_getSupportedServiceNames() throw (RuntimeException) {
    Sequence < ::rtl::OUString > aRet(1);
    ::rtl::OUString* pArray = aRet.getArray();
    pArray[0] =  OUString ( SERVICE_NAME );
    return aRet;
}


Reference< XInterface > SAL_CALL Addon_createInstance( const Reference< XComponentContext > & rContext) throw( Exception ) {
    return (cppu::OWeakObject*) new Addon( rContext );
}


::rtl::OUString SAL_CALL Addon::getImplementationName() throw (RuntimeException) {
    return Addon_getImplementationName();
}

sal_Bool SAL_CALL Addon::supportsService( const ::rtl::OUString& rServiceName ) throw (RuntimeException) {
    return cppu::supportsService(this, rServiceName);
}


Sequence< ::rtl::OUString > SAL_CALL Addon::getSupportedServiceNames(  ) throw (RuntimeException) {
    return Addon_getSupportedServiceNames();
}

