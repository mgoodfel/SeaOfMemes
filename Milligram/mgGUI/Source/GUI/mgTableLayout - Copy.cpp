/*
  Copyright (C) 1995-2012 by Michael J. Goodfellow

  This source code is distributed for free and may be modified, redistributed, and
  incorporated in other projects (commercial, non-commercial and open-source)
  without restriction.  No attribution to the author is required.  There is
  no requirement to make the source code available (no share-alike required.)

  This source code is distributed "AS IS", with no warranty expressed or implied.
  The user assumes all risks related to quality, accuracy and fitness of use.

  Except where noted, this source code is the sole work of the author, but it has 
  not been checked for any intellectual property infringements such as copyrights, 
  trademarks or patents.  The user assumes all legal risks.  The original version 
  may be found at "http://www.sea-of-memes.com".  The author is not responsible 
  for subsequent alterations.

  Retain this copyright notice and add your own copyrights and revisions above
  this notice.
*/

#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

// Table-style layout manager
// 
// bug: setRowBottomInset(10) on last row, did not have extra space at bottom.
// TableMaxWidth < TableMinWidth in preferredSize

#include "GUI/mgFrame.h"
#include "GUI/mgTableLayout.h"

const int MG_INIT_COLUMN_COUNT = 20;
const int MG_INC_COLUMN_COUNT = 20;

// a cell of the table
class mgTableCell
{
public:
  mgControl* m_child;           // contents of cell

  mgTableAlign m_hAlign;        // horizontal alignment
  mgTableAlign m_vAlign;        // vertical alignment
  int m_colSpan;                // number of columns spanned by cell
  int m_rowSpan;                // number of rows spanned by cell
                                
  int m_leftInset;              // insets around border
  int m_rightInset;
  int m_topInset;
  int m_bottomInset;

  const mgFrame* m_frame;       // cell frame

  int m_hWeight;                // weight when stretching horizontally
  int m_vWeight;                // weight when stretching vertically

  int m_minWidth;               // min formatted width
  int m_maxWidth;               // max formatted width

  int m_minHeight;              // min formatted height
  int m_maxHeight;              // max formatted height

  // constructor
  mgTableCell()
  {
    m_child = NULL;
    m_leftInset = 0;
    m_rightInset = 0;
    m_topInset = 0;
    m_bottomInset = 0;
    m_frame = NULL;

    m_hWeight = 1;
    m_vWeight = 1;
  }
};

// a row of the table
class mgTableRow
{
public:
  mgPtrArray m_columns;             // first cell in row
  
  // destructor
  virtual ~mgTableRow();
};

// per column data
class mgColumnData
{
public:
  int m_hWeight;
  int m_leftInset;
  int m_rightInset;

  // constructor
  mgColumnData()
  {
    m_hWeight = 1;
    m_leftInset = 0;
    m_rightInset = 0;
  }
};

//--------------------------------------------------------------
// destructor
mgTableRow::~mgTableRow()
{
  for (int i = 0; i < m_columns.length(); i++)
  {
    mgTableCell* cell = (mgTableCell*) m_columns[i];
    delete cell;
  }
  m_columns.removeAll();
}

//--------------------------------------------------------------
// constructor
mgTableLayout::mgTableLayout(
  mgControl* parent) 
: mgLayoutManager(parent)
{
  m_frame = NULL;
  m_row = NULL;
  m_cell = NULL;

  // default table is min width
  m_rowCount = m_colCount = -1;

  m_colUsed = NULL;
  m_colTargets = NULL;
  m_colWeights = NULL;

  m_minWidths = NULL;
  m_maxWidths = NULL;
  m_minHeights = NULL;
  m_maxHeights = NULL;
  m_rowWeights = NULL;
}
  
//--------------------------------------------------------------
// destructor
mgTableLayout::~mgTableLayout() 
{
  // delete all rows
  removeAll();
  
  for (int i = 0; i < m_colData.length(); i++)
  {
    mgColumnData* colData = (mgColumnData*) m_colData.getAt(i);
    delete colData;
  }
  m_colData.removeAll();
  
  termLayout();
}
  
//--------------------------------------------------------------
// set outside frame
void mgTableLayout::setFrame(
  const mgFrame* frame)
{
  m_frame = frame;
  controlResized();
}

//--------------------------------------------------------------
// add a row
void mgTableLayout::newRow() 
{
  // create new row, add to table
  mgTableRow* row = new mgTableRow();

  m_rows.add(row);

  // set current row
  m_row = row;
  m_cell = NULL;  // no current cell
  m_colNum = 0;

  m_rowVWeight = 1;
  m_rowTopInset = 0;
  m_rowBottomInset = 0;
}
  
//--------------------------------------------------------------
// end a row
void mgTableLayout::endRow() 
{
  // force create of new row
  m_row = NULL;
}
  
//--------------------------------------------------------------
// add a cell to the table
void mgTableLayout::addCell(
  mgControl* child,
  mgTableAlign hAlign,
  mgTableAlign vAlign,
  int rowSpan,
  int colSpan) 
{
  // advance colnum by span of last cell
  if (m_cell != NULL)
    m_colNum += m_cell->m_colSpan;

  if (m_row == NULL)
    newRow();

  // allocate a cell and chain it in
  mgTableCell* cell = new mgTableCell();
  cell->m_child = child;
  cell->m_rowSpan = rowSpan;
  cell->m_colSpan = colSpan;
  cell->m_hAlign = hAlign;
  cell->m_vAlign = vAlign;

  cell->m_maxWidth = -1;
  cell->m_minWidth = -1;

  cell->m_topInset = m_rowTopInset;
  cell->m_bottomInset = m_rowBottomInset;
  cell->m_vWeight = m_rowVWeight;

  // extend column data array to include spanned cells
  while (m_colData.length() <= (m_colNum+colSpan))
    m_colData.add(new mgColumnData());

  // default column parameters based on coldata
  mgColumnData* colData = (mgColumnData*) m_colData.getAt(m_colNum);
  cell->m_leftInset = colData->m_leftInset;
  cell->m_rightInset = colData->m_rightInset;
  cell->m_hWeight = colData->m_hWeight;

  // add cell to row and make current
  m_row->m_columns.add(cell);
  m_cell = cell;
}
  
//--------------------------------------------------------------
// set frame around current cell
void mgTableLayout::setCellFrame(
  const mgFrame* frame)
{
  if (m_cell != NULL)
    m_cell->m_frame = frame;
}
    
//--------------------------------------------------------------
// set sizing weight for row
void mgTableLayout::setRowWeight(
  int weight) 
{
  if (m_row == NULL)
    newRow();
  m_rowVWeight = weight;
}
  
//--------------------------------------------------------------
// set top inset for row
void mgTableLayout::setRowTopInset(
  int top) 
{
  if (m_row == NULL)
    newRow();
  m_rowTopInset = top;
}
  
//--------------------------------------------------------------
// set bottom inset for row
void mgTableLayout::setRowBottomInset(
  int bottom) 
{
  if (m_row == NULL)
    newRow();
  m_rowBottomInset = bottom;
}
  
//--------------------------------------------------------------
// set sizing weight for cell
void mgTableLayout::setColWeight(
  int weight) 
{
  if (m_cell == NULL)
    throw new mgException("setColWeight called before cell created");
    
  // set weight for all spanned columns
  for (int i = 0; i < m_cell->m_colSpan; i++)
  {
    // set column defaults, and apply to current cell
    mgColumnData* colData = (mgColumnData*) m_colData.getAt(m_colNum+i);
    colData->m_hWeight = weight;
  }

  // set weight for current cell
  m_cell->m_hWeight = weight;
}
  
//--------------------------------------------------------------
// set left insets for column
void mgTableLayout::setColLeftInset(
  int left) 
{
  if (m_cell == NULL)
    throw new mgException("setColLeftInset called before cell created");
    
  // set insets for all spanned columns
  for (int i = 0; i < m_cell->m_colSpan; i++)
  {
    mgColumnData* colData = (mgColumnData*) m_colData.getAt(m_colNum+i);
    colData->m_leftInset = left;
  }

  // set for current cell
  m_cell->m_leftInset = left;
}
  
//--------------------------------------------------------------
// set right insets for column
void mgTableLayout::setColRightInset(
  int right) 
{
  if (m_cell == NULL)
    throw new mgException("setColRightInset called before cell created");
    
  // set insets for all spanned columns
  for (int i = 0; i < m_cell->m_colSpan; i++)
  {
    mgColumnData* colData = (mgColumnData*) m_colData.getAt(m_colNum+i);
    colData->m_rightInset = right;
  }

  // set for current cell
  m_cell->m_rightInset = right;
}
  
//--------------------------------------------------------------
// find cell containing child
void* mgTableLayout::findChild(
  mgControl* child)
{
  for (int i = 0; i < m_rows.length(); i++)
  {
    mgTableRow* row = (mgTableRow*) m_rows.getAt(i);

    for (int j = 0; j < row->m_columns.length(); j++)
    {
      mgTableCell* cell = (mgTableCell*) row->m_columns.getAt(j);
      if (cell->m_child == child)
        return (void*) cell;
    }
  }
  return NULL;
}

//--------------------------------------------------------------
// remove child control from layout
void mgTableLayout::removeChild(
  mgControl* child)
{
  mgTableCell* cell = (mgTableCell*) findChild(child);
  if (cell != NULL)
    cell->m_child = NULL;
}

//--------------------------------------------------------------
// replace child in layout
void mgTableLayout::setChild(
  void* voidCell,
  mgControl* child)
{
  mgTableCell* cell = (mgTableCell*) voidCell;
  if (cell != NULL)
    cell->m_child = child;
}

//--------------------------------------------------------------
// remove all children
void mgTableLayout::removeAll() 
{
  // delete all rows
  for (int i = 0; i < m_rows.length(); i++)
  {
    mgTableRow* row = (mgTableRow*) m_rows.getAt(i);
    delete row;
  }
  m_rows.removeAll();
  m_row = NULL;
  m_cell = NULL;
}
  
//--------------------------------------------------------------
// get minimum size of layout
void mgTableLayout::minimumSize(
  mgDimension& size) 
{
  // =-= if we've done layout and not added any new cells, don't have to do this again
  initLayout();
  initColWidths();
  getRowHeights();
  getTableSize();
  size.m_width = m_tableMinWidth;
  size.m_height = m_tableMinHeight;

  // expand size by frame
  if (m_frame != NULL)
  {
    mgDimension extSize;
    m_frame->getOutsideSize(size, extSize);
    size = extSize;
  }
}
  
//--------------------------------------------------------------
// get preferred size
void mgTableLayout::preferredSize(
  mgDimension& size) 
{
  // =-= if we've done layout and not added any new cells, don't have to do this again
  initLayout();
  initColWidths();
  getTableSize();

  /*
    By stretching table to its max (preferred) width, we reformat cells.  Since wider
    text is shorter, this changes the row heights.
  */
  stretchCols(m_tableMaxWidth, 0, m_colCount, false);
  getRowHeights();
  getTableSize();
  
  size.m_width = max(m_tableMaxWidth, m_tableMinWidth);
  size.m_height = m_tableMaxHeight;

  // expand size by frame
  if (m_frame != NULL)
  {
    mgDimension extSize;
    m_frame->getOutsideSize(size, extSize);
    size = extSize;
  }
  mgDebug("preferredSize = (%d, %d)", size.m_width, size.m_height);
}
  
//--------------------------------------------------------------
// return preferred size at width
BOOL mgTableLayout::preferredSizeAtWidth(
  int width,
  mgDimension& size) 
{
  // reduce requested size by frame
  if (m_frame != NULL)
  {
    mgRectangle inside(0, 0, width, 0);
    m_frame->getInsideRect(inside);
    width = inside.m_width;
  }

  // =-= if we've done layout and not added any new cells, don't have to do this again
  initLayout();
  initColWidths();
  getTableSize();

  /*
    By stretching table to its requested width, we reformat cells.  Since wider
    text is shorter, this changes the row heights.
  */
  stretchCols(width, 0, m_colCount, false);
  getRowHeights();
  getTableSize();
  
  size.m_width = m_tableMaxWidth;
  size.m_height = m_tableMaxHeight;
  
  // expand size by frame
  if (m_frame != NULL)
  {
    mgDimension extSize;
    m_frame->getOutsideSize(size, extSize);
    size = extSize;
  }
  mgDebug("preferredSizeAtWidth(%d) = (%d, %d)", width, size.m_width, size.m_height);
  return true;
}
  
//--------------------------------------------------------------
// update layout after resize of control
void mgTableLayout::controlResized()
{
  // if called before any children added or size set, nothing to do
  mgDimension size;
  m_parent->getSize(size);
  if (m_rows.length() == 0 || size.m_width == 0 || size.m_height == 0)
    return;

  // reduce dimensions by frame
  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (m_frame != NULL)
    m_frame->getInsideRect(inside);
  
  // =-= if we've done layout and not added any new cells, don't have to do this again
  initLayout();
  initColWidths();

  /*
    By stretching table to its requested width, we reformat cells.  Since wider
    text is shorter, this changes the row heights.
  */
  stretchCols(inside.m_width, 0, m_colCount, false);
  getRowHeights();
  stretchRows(inside.m_height, 0, m_rowCount, false);
  positionChildren(inside.m_x, inside.m_y);
}
  
//--------------------------------------------------------------
// paint background graphics
void mgTableLayout::paintBackground(
  mgContext* gc) 
{
  mgDimension size;
  m_parent->getSize(size);
  if (size.m_width == 0 || size.m_height == 0)
    return;  // nothing to do
  
  // reduce dimensions by frame
  mgRectangle inside(0, 0, size.m_width, size.m_height);

  if (m_frame != NULL)
  {
    m_frame->paintBackground(gc, inside);
    m_frame->getInsideRect(inside);
  }
  int originX = inside.m_x;
  int originY = inside.m_y;

  // initialize row/col span tracking
  memset(m_colUsed, 0, sizeof(int)*m_colCount);

  int y = 0;
  int r = 0;
  for (int i = 0; i < m_rows.length(); i++)
  {
    mgTableRow* row = (mgTableRow*) m_rows.getAt(i);

    int x = 0;
    int c = 0;
    for (int j = 0; j < row->m_columns.length(); j++)
    {
      mgTableCell* cell = (mgTableCell*) row->m_columns.getAt(j);

      // skip to next available cell
      while (m_colUsed[c] > 0)
        x += m_minWidths[c++];

      // figure width of cell spanning multiple columns
      int cellWidth = 0;
      for (int k = 0; k < cell->m_colSpan; k++)
      {
        cellWidth += m_minWidths[c];

        // set used for RowSpan rows
        m_colUsed[c] = cell->m_rowSpan;
        c++;
      }

      // figure height for cells spanning multiple rows
      int cellHeight = 0;
      for (int k = 0; k < cell->m_rowSpan; k++)
        cellHeight += m_minHeights[r+k];

      int fmtWidth = min(cellWidth, cell->m_maxWidth);
      int fmtHeight = min(cellHeight, cell->m_maxHeight);

      // figure vertical alignment
      int yAdjust = 0;
      switch (cell->m_vAlign)
      {
        case mgTableAlignTop:
          yAdjust = 0;
          break;
        case mgTableAlignBottom:
          yAdjust = cellHeight - fmtHeight;
          break;
        case mgTableAlignVCenter:
          yAdjust = (cellHeight - fmtHeight)/2;
          break;
        case mgTableAlignVFill:
          yAdjust = 0;
          fmtHeight = cellHeight;
          break;
      }

      // figure horizontal alignment
      int xAdjust = 0;
      switch (cell->m_hAlign)
      {
        case mgTableAlignLeft:
          xAdjust = 0;
          break;
        case mgTableAlignRight:
          xAdjust = cellWidth - fmtWidth;
          break;
        case mgTableAlignHCenter:
          xAdjust = (cellWidth - fmtWidth)/2;
          break;
        case mgTableAlignHFill:
          xAdjust = 0;
          fmtWidth = cellWidth;
          break;
      }

      // adjust for cell bounds
      if (cell->m_frame != NULL)
      {
        inside.m_x = originX + x + xAdjust + cell->m_leftInset;
        inside.m_y = originY + y + yAdjust + cell->m_topInset;
        inside.m_width = fmtWidth - (cell->m_leftInset+cell->m_rightInset);
        inside.m_height = fmtHeight - (cell->m_topInset+cell->m_bottomInset);
        
        cell->m_frame->paintBackground(gc, inside);
      }
        
      x += cellWidth;
    }
    // decrement all used cols
    for (int k = 0; k < c; k++)
      m_colUsed[k] = max(0, m_colUsed[k]-1);

    y += m_minHeights[r];

    r++;
  }
}

//--------------------------------------------------------------
// paint foreground graphics
void mgTableLayout::paintForeground(
  mgContext* gc) 
{
  mgDimension size;
  m_parent->getSize(size);
  if (size.m_width == 0 || size.m_height == 0)
    return;  // nothing to do
  
  // reduce dimensions by frame
  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (m_frame != NULL)
  {
    m_frame->paintForeground(gc, inside);
    m_frame->getInsideRect(inside);
  }

  int originX = inside.m_x;
  int originY = inside.m_y;

  // initialize row/col span tracking
  memset(m_colUsed, 0, sizeof(int)*m_colCount);

  int y = 0;
  int r = 0;
  for (int i = 0; i < m_rows.length(); i++)
  {
    mgTableRow* row = (mgTableRow*) m_rows.getAt(i);

    int x = 0;
    int c = 0;
    for (int j = 0; j < row->m_columns.length(); j++)
    {
      mgTableCell* cell = (mgTableCell*) row->m_columns.getAt(j);

      // skip to next available cell
      while (m_colUsed[c] > 0)
        x += m_minWidths[c++];

      // figure width of cell spanning multiple columns
      int cellWidth = 0;
      for (int k = 0; k < cell->m_colSpan; k++)
      {
        cellWidth += m_minWidths[c];

        // set used for RowSpan rows
        m_colUsed[c] = cell->m_rowSpan;
        c++;
      }

      // figure height for cells spanning multiple rows
      int cellHeight = 0;
      for (int k = 0; k < cell->m_rowSpan; k++)
        cellHeight += m_minHeights[r+k];

      int fmtWidth = min(cellWidth, cell->m_maxWidth);
      int fmtHeight = min(cellHeight, cell->m_maxHeight);

      // figure vertical alignment
      int yAdjust = 0;
      switch (cell->m_vAlign)
      {
        case mgTableAlignTop:
          yAdjust = 0;
          break;
        case mgTableAlignBottom:
          yAdjust = cellHeight - fmtHeight;
          break;
        case mgTableAlignVCenter:
          yAdjust = (cellHeight - fmtHeight)/2;
          break;
        case mgTableAlignVFill:
          yAdjust = 0;
          fmtHeight = cellHeight;
          break;
      }

      // figure horizontal alignment
      int xAdjust = 0;
      switch (cell->m_hAlign)
      {
        case mgTableAlignLeft:
          xAdjust = 0;
          break;
        case mgTableAlignRight:
          xAdjust = cellWidth - fmtWidth;
          break;
        case mgTableAlignHCenter:
          xAdjust = (cellWidth - fmtWidth)/2;
          break;
        case mgTableAlignHFill:
          xAdjust = 0;
          fmtWidth = cellWidth;
          break;
      }

      // adjust for cell bounds
      if (cell->m_frame != NULL)
      {
        inside.m_x = originX + x + xAdjust + cell->m_leftInset;
        inside.m_y = originY + y + yAdjust + cell->m_topInset;
        inside.m_width = fmtWidth - (cell->m_leftInset+cell->m_rightInset);
        inside.m_height = fmtHeight - (cell->m_topInset+cell->m_bottomInset);
        
        cell->m_frame->paintForeground(gc, inside);
      }
        
      x += cellWidth;
    }
    // decrement all used cols
    for (int k = 0; k < c; k++)
      m_colUsed[k] = max(0, m_colUsed[k]-1);

    y += m_minHeights[r];

    r++;
  }
}

//--------------------------------------------------------------
// initialize layout
void mgTableLayout::initLayout()
{
  if (m_colUsed == NULL)
  {
    getGridSize();
    m_colWeights = new int[m_colCount];
    m_colTargets = new int[m_colCount];
    m_minWidths = new int[m_colCount];
    m_maxWidths = new int[m_colCount];
    m_minHeights = new int[m_rowCount];
    m_maxHeights = new int[m_rowCount];
    m_rowWeights = new int[m_rowCount];
  }

  memset(m_colWeights, 0, sizeof(int)*m_colCount);
  memset(m_colTargets, 0, sizeof(int)*m_colCount);
  memset(m_minWidths, 0, sizeof(int)*m_colCount);
  memset(m_maxWidths, 0, sizeof(int)*m_colCount);
  memset(m_minHeights, 0, sizeof(int)*m_rowCount);
  memset(m_maxHeights, 0, sizeof(int)*m_rowCount);
  memset(m_rowWeights, 0, sizeof(int)*m_rowCount);
}

//--------------------------------------------------------------
// free layout state
void mgTableLayout::termLayout()
{
  delete m_colUsed;
  m_colUsed = NULL;

  delete m_colWeights;
  m_colWeights = NULL;

  delete m_colTargets;
  m_colTargets = NULL;

  delete m_minWidths;
  m_minWidths = NULL;
  delete m_maxWidths;
  m_maxWidths = NULL;

  delete m_minHeights;
  m_minHeights = NULL;
  delete m_maxHeights;
  m_maxHeights = NULL;

  delete m_rowWeights;
  m_rowWeights = NULL;
}  

//--------------------------------------------------------------
// find the number of rows an columns
void mgTableLayout::getGridSize()
{
  if (m_rowCount != -1)
    return;  // already measured

  m_rowCount = m_rows.length();
  m_colCount = 0;

  // keep active columns, to handle rowspan
  int usedMax = MG_INIT_COLUMN_COUNT;
  m_colUsed = new int[usedMax];
  memset(m_colUsed, 0, usedMax*sizeof(int));

  for (int i = 0; i < m_rows.length(); i++)
  {
    mgTableRow* row = (mgTableRow*) m_rows.getAt(i);
    int c = 0;

    for (int j = 0; j < row->m_columns.length(); j++)
    {
      mgTableCell* cell = (mgTableCell*) row->m_columns.getAt(j);

      // skip to next available cell
      while (c < usedMax && m_colUsed[c] > 0)
        c++;

      if (c+cell->m_colSpan >= usedMax)
      {
        // reallocate used array
        usedMax += MG_INC_COLUMN_COUNT+cell->m_colSpan;
        int* newUsed = new int[usedMax];
        memset(newUsed, 0, usedMax*sizeof(int));
        memcpy(newUsed, m_colUsed, sizeof(int)*c);
        delete m_colUsed;
        m_colUsed = newUsed;
      }

      // mark used columns
      for (int k = 0; k < cell->m_colSpan; k++)
        m_colUsed[c++] = cell->m_rowSpan;
    }
    m_colCount = max(m_colCount, c);

    // decrement all used cols
    for (int k = 0; k < m_colCount; k++)
      m_colUsed[k] = max(0, m_colUsed[k]-1);
  }
}

//--------------------------------------------------------------
// initialize col width ranges from cell width ranges
void mgTableLayout::initColWidths()
{
  // initialize row/col span tracking
  memset(m_colUsed, 0, sizeof(int)*m_colCount);

  // initialize weights and widths
  memset(m_colWeights, 0, sizeof(int)*m_colCount);
  memset(m_minWidths, 0, sizeof(int)*m_colCount);
  memset(m_maxWidths, 0, sizeof(int)*m_colCount);

  int r = 0;
  for (int i = 0; i < m_rows.length(); i++)
  {
    mgTableRow* row = (mgTableRow*) m_rows.getAt(i);
    m_rowWeights[i] = 0;

    int c = 0;
    for (int j = 0; j < row->m_columns.length(); j++)
    {
      mgTableCell* cell = (mgTableCell*) row->m_columns[j];

      // skip to next available cell
      while (m_colUsed[c] > 0)
        c++;

      // measure cell if we haven't already done so (or if invalid)
      if (cell->m_minWidth == -1) //  || !cell.m_child.isValid())
      {
        int horzInset = cell->m_leftInset + cell->m_rightInset;
        int vertInset = cell->m_topInset + cell->m_bottomInset;

        mgDimension minSize(0,0);
        mgDimension maxSize(0,0);
        if (cell->m_child != NULL)
        {
          cell->m_child->minimumSize(minSize);
          cell->m_child->preferredSize(maxSize);
mgDebug("%08x minSize = (%d, %d) maxSize = (%d, %d)", cell->m_child, 
  minSize.m_width, minSize.m_height,
  maxSize.m_width, maxSize.m_height);
        }

        if (cell->m_frame != NULL)
        {
          mgDimension extSize;
          cell->m_frame->getOutsideSize(minSize, extSize);
          minSize = extSize;
          cell->m_frame->getOutsideSize(maxSize, extSize);
          maxSize = extSize;
        }

        /*
          Cells contain controls, which can return whatever sizes they like for 
          min and max.  So we must assume that min and max sizes have no relation 
          to one another.  For example, a narrow text column (min size) could be 
          taller than a wide text column (max size).

          Max size is preferredSize, so we should try for this width, and only
          reduce width down as far as minSize width.
        */
        cell->m_minWidth = minSize.m_width + horzInset;
        cell->m_minHeight = minSize.m_height + vertInset;

        cell->m_maxWidth = maxSize.m_width + horzInset;
        cell->m_maxHeight = maxSize.m_height + vertInset;
        
      }
      m_colWeights[c] = max(m_colWeights[c], cell->m_hWeight);
      m_rowWeights[i] = max(m_rowWeights[i], cell->m_vWeight);

      // leave spanning columns for later
      if (cell->m_colSpan == 1)
      {
        m_minWidths[c] = max(m_minWidths[c], cell->m_minWidth);
        m_maxWidths[c] = max(m_maxWidths[c], cell->m_maxWidth);
      }

      // make sure row and col spans are legal
      cell->m_colSpan = min(cell->m_colSpan, m_colCount - c);
      cell->m_rowSpan = min(cell->m_rowSpan, m_rowCount - r);

      // mark used columns
      for (int k = 0; k < cell->m_colSpan; k++)
        m_colUsed[c++] = cell->m_rowSpan;
    }

    // decrement all used cols
    for (int k = 0; k < m_colCount; k++)
      m_colUsed[k] = max(0, m_colUsed[k]-1);

    r++;
  }

  // initialize row/col span tracking
  memset(m_colUsed, 0, sizeof(int)*m_colCount);

  // now that we know individual column widths, handle the spanning columns
  for (int i = 0; i < m_rows.length(); i++)
  {
    mgTableRow* row = (mgTableRow*) m_rows.getAt(i);
    int c = 0;
    for (int j = 0; j < row->m_columns.length(); j++)
    {
      mgTableCell* cell = (mgTableCell*) row->m_columns.getAt(j);
      // skip to next available cell
      while (m_colUsed[c] > 0)
        c++;

      // widen cells under a colspan to fit content
      if (cell->m_colSpan > 1)
      {
        // stretch the columns spanned by the cell
        stretchCols(cell->m_minWidth, c, cell->m_colSpan, true);
      }

      // mark used columns
      for (int k = 0; k < cell->m_colSpan; k++)
        m_colUsed[c++] = cell->m_rowSpan;
    }
    // decrement all used cols
    for (int k = 0; k < m_colCount; k++)
      m_colUsed[k] = max(0, m_colUsed[k]-1);
  }
}

//--------------------------------------------------------------
// stretch a range of cells to be at least target width
void mgTableLayout::stretchCols(
  int targetWidth,             // size we need
  int col,                     // starting column
  int colSpan,                 // count of spanning columns
  BOOL fill)                   // fill target width
{
  memset(m_colTargets, 0, sizeof(int)*m_colCount);

  /*
    We have zero weight columns which should always be min width.
    Non-zero weight columns should divide remaining width by the
    ratio of their weight to total weight.
  */
  int allMinWidth = 0;
  int zeroMaxWidth = 0;
  int nonZeroMinWidth = 0;
  int nonZeroWeight = 0;
  for (int k = col; k < col+colSpan; k++)
  {
    allMinWidth += m_minWidths[k];
    if (m_colWeights[k] == 0)
      zeroMaxWidth += m_maxWidths[k];
    else
    {
      nonZeroWeight += m_colWeights[k];
      nonZeroMinWidth += m_minWidths[k];
    }
  }

  // if total min width wider than requested size, nothing to do
  if (targetWidth <= allMinWidth)
    return;

  /*
    First, bring all zero-weighted columns up to their
    preferred (max) size.  They will have max total width
    minus the min width of weighted columns.
  */
  int zeroTotalWidth = targetWidth - nonZeroMinWidth;
  int zeroUsedWidth = 0;  // amount of space used
  if (zeroMaxWidth > 0)
  {
    for (int k = col; k < col+colSpan; k++)
    {
      if (m_colWeights[k] == 0)
      {
        // take percentage of avail width based on preferred size
        m_colTargets[k] = (m_maxWidths[k] * zeroTotalWidth)/zeroMaxWidth;
      }
      else m_colTargets[k] = -1;
    }
    distribWidth(zeroTotalWidth, col, colSpan);

    // constrain the widths to be less than max, get sum of widths
    for (int k = col; k < col+colSpan; k++)
    {
      if (m_colWeights[k] == 0)
      {
        m_minWidths[k] = min(m_minWidths[k], m_maxWidths[k]);
        zeroUsedWidth += m_minWidths[k];
      }
    }
  }

  // calculate remaining space
  int nonZeroTotalWidth = targetWidth - zeroUsedWidth;
  // distribute this among non-zero weight columns to reach
  // desired proportion of total width
  int nonZeroUsedWidth = 0;
  if (nonZeroWeight > 0 && nonZeroTotalWidth > 0)
  {
    for (int k = col; k < col+colSpan; k++)
    {
      if (m_colWeights[k] != 0)
      {
        // take percentage of avail width based on weight
        m_colTargets[k] = (m_colWeights[k] * nonZeroTotalWidth)/nonZeroWeight;
      }
      else m_colTargets[k] = -1;
    }
    nonZeroUsedWidth = distribWidth(nonZeroTotalWidth, col, colSpan);
  }

  if (fill)
  {
    // distribute remaining space over all columns, pushing them towards
    // equal size.
    if (targetWidth > (zeroUsedWidth + nonZeroUsedWidth))
    {
      for (int k = col; k < col+colSpan; k++)
      {
        m_colTargets[k] = (targetWidth / colSpan);
      }
      distribWidth(targetWidth, col, colSpan);
    }
  }
}

//--------------------------------------------------------------
// distribute added column width
int mgTableLayout::distribWidth(
  int totalWidth,                // size we need
  int col,                       // starting column
  int colSpan)                   // count of spanning columns
{
  int totalGrowth = 0;
  int remaining = totalWidth;
  for (int k = col; k < col+colSpan; k++)
  {
    if (m_colTargets[k] != -1)
    {
      int nGrowth = m_colTargets[k] - m_minWidths[k];
      if (nGrowth > 0)
        totalGrowth += nGrowth;
      remaining -= m_minWidths[k];
    }
  }

  int used = 0;
  for (int k = col; k < col+colSpan; k++)
  {
    if (m_colTargets[k] != -1)
    {
      if (totalGrowth > 0)
      {
        int nGrowth = m_colTargets[k] - m_minWidths[k];
        if (nGrowth > 0)
          m_minWidths[k] += (nGrowth * remaining)/totalGrowth;
      }

      used += m_minWidths[k];
    }
  }
  return used;
}

//--------------------------------------------------------------
// get row heights
void mgTableLayout::getRowHeights()
{
  // initialize row/col span tracking
  memset(m_colUsed, 0, sizeof(int)*m_colCount);

  int r = 0;
  for (int i = 0; i < m_rows.length(); i++)
  {
    mgTableRow* row = (mgTableRow*) m_rows.getAt(i);
    int c = 0;
    for (int j = 0; j < row->m_columns.length(); j++)
    {
      mgTableCell* cell = (mgTableCell*) row->m_columns.getAt(j);

      // skip to next available cell
      while (m_colUsed[c] > 0)
        c++;

      // get width for cells, including spans
      int cellWidth = 0;
      for (int k = 0; k < cell->m_colSpan; k++)
      {
        cellWidth += m_minWidths[c];
        // set used for RowSpan rows
        m_colUsed[c] = cell->m_rowSpan;
        c++;
      }

      // if implements size at width, override default preferred size
      int horzInset = cell->m_leftInset + cell->m_rightInset;
      int vertInset = cell->m_topInset + cell->m_bottomInset;

      mgDimension childSize;
      if (cell->m_child->preferredSizeAtWidth(cellWidth - horzInset, childSize))
      {
        cell->m_maxHeight = vertInset + childSize.m_height;
        mgDebug("%08x sizeAtWidth(%d) = (%d, %d)", cell->m_child, cellWidth-horzInset,
                 childSize.m_width, childSize.m_height);
      }

      // handle spanning rows later
      if (cell->m_rowSpan == 1)
      {
        // cells can trade width for height (as with tall+narrow vs. wide+short text)
        // so min Height is height when min width used, and maxHeight is height when
        // maxWidth used.
        m_minHeights[r] = max(m_minHeights[r], cell->m_minHeight);
        m_maxHeights[r] = max(m_maxHeights[r], cell->m_maxHeight);
      }
    }
    // decrement all used cols
    for (int k = 0; k < m_colCount; k++)
      m_colUsed[k] = max(0, m_colUsed[k]-1);

    mgDebug("row height = %d to %d", m_minHeights[r], m_maxHeights[r]);
    r++;
  }

  // handle spanning rows
  r = 0;
  for (int i = 0; i < m_rows.length(); i++)
  {
    mgTableRow* row = (mgTableRow*) m_rows.getAt(i);
    for (int j = 0; j < row->m_columns.length(); j++)
    {
      mgTableCell* cell = (mgTableCell*) row->m_columns.getAt(j);

      // if spans multiple rows
      if (cell->m_rowSpan > 1)
      {
        // stretch rows to handle size of cell
        stretchRows(cell->m_minHeight, r, cell->m_rowSpan, true);
      }
    }
    r++;
  }
}

//--------------------------------------------------------------
// stretch rows to at least requested height
void mgTableLayout::stretchRows(
  int targetHeight,
  int row,
  int rowSpan,
  BOOL fill)
{
  int* rowTargets = new int[m_rowCount];

  // figure current total of spanned rows
  int spanMinHeight = 0;
  int zeroMaxHeight = 0;
  int nonZeroMinHeight = 0;
  int nonZeroWeight = 0;
  for (int k = row; k < row+rowSpan; k++)
  {
    spanMinHeight += m_minHeights[k];
    if (m_rowWeights[k] == 0)
      zeroMaxHeight += m_maxHeights[k];
    else
    {
      nonZeroWeight += m_rowWeights[k];
      nonZeroMinHeight += m_minHeights[k];
    }
  }

  // if already taller than requested size
  if (targetHeight <= spanMinHeight)
  {
    delete rowTargets;
    return;  // nothing to do
  }

  // first, bring all zero-weighted rows up to their
  // preferred (max) size.  They will have max total height
  // minus the min height of weighted rows.
  int zeroTotalHeight = targetHeight - nonZeroMinHeight;
  int zeroUsedHeight = 0;  // amount of space used
  if (zeroMaxHeight > 0)
  {
    for (int k = row; k < row+rowSpan; k++)
    {
      if (m_rowWeights[k] == 0)
      {
        // take percentage of avail width based on preferred size
        rowTargets[k] = (m_maxHeights[k] * zeroTotalHeight)/zeroMaxHeight;
      }
      else rowTargets[k] = -1;
    }
    distribHeight(zeroTotalHeight, row, rowSpan, rowTargets);

    // constrain the heights to be less than max, get sum of heights
    for (int k = row; k < row+rowSpan; k++)
    {
      if (m_rowWeights[k] == 0)
      {
        m_minHeights[k] = min(m_minHeights[k], m_maxHeights[k]);
        zeroUsedHeight += m_minHeights[k];
      }
    }
  }

  // calculate remaining space
  int nonZeroTotalHeight = targetHeight - zeroUsedHeight;
  // distribute this among non-zero weight rows to reach
  // desired proportion of total height
  int nonZeroUsedHeight = 0;
  if (nonZeroWeight > 0 && nonZeroTotalHeight > 0)
  {
    for (int k = row; k < row+rowSpan; k++)
    {
      if (m_rowWeights[k] != 0)
      {
        // take percentage of avail width based on weight
        rowTargets[k] = (m_rowWeights[k] * nonZeroTotalHeight)/nonZeroWeight;
      }
      else rowTargets[k] = -1;
    }
    nonZeroUsedHeight = distribHeight(nonZeroTotalHeight, row, rowSpan, rowTargets);
  }

  if (fill)
  {
    // distribute remaining space over all rows, pushing them towards
    // equal size.
    if (targetHeight > (zeroUsedHeight + nonZeroUsedHeight))
    {
      for (int k = row; k < row+rowSpan; k++)
      {
        rowTargets[k] = (targetHeight / rowSpan);
      }
      distribHeight(targetHeight, row, rowSpan, rowTargets);
    }
  }
  delete rowTargets;
}

//--------------------------------------------------------------
// distribute added row height
int mgTableLayout::distribHeight(
  int totalHeight,                // size we need
  int row,                        // starting column
  int rowSpan,                    // count of spanning columns
  int* targetHeights)             // weights to use
{
  int totalGrowth = 0;
  int remaining = totalHeight;
  for (int k = row; k < row+rowSpan; k++)
  {
    if (targetHeights[k] != -1)
    {
      int nGrowth = targetHeights[k] - m_minHeights[k];
      if (nGrowth > 0)
        totalGrowth += nGrowth;
      remaining -= m_minHeights[k];
    }
  }

  int used = 0;
  for (int k = row; k < row+rowSpan; k++)
  {
    if (targetHeights[k] != -1)
    {
      if (totalGrowth > 0)
      {
        int nGrowth = targetHeights[k] - m_minHeights[k];
        if (nGrowth > 0)
          m_minHeights[k] += (nGrowth * remaining)/totalGrowth;
      }

      used += m_minHeights[k];
    }
  }
  return used;
}

//--------------------------------------------------------------
// return current table width range
void mgTableLayout::getTableSize()
{
  // get table overall width range
  m_tableMinWidth = 0;
  m_tableMaxWidth = 0;
  for (int j = 0; j < m_colCount; j++)
  {
    m_tableMinWidth += m_minWidths[j];
    m_tableMaxWidth += m_maxWidths[j];
  }

  // get table height
  m_tableMinHeight = 0;
  m_tableMaxHeight = 0;
  for (int i = 0; i < m_rowCount; i++)
  {
    m_tableMinHeight += m_minHeights[i];
    m_tableMaxHeight += m_maxHeights[i];
  }
}

//--------------------------------------------------------------
// position cells of table
void mgTableLayout::positionChildren(
  int originX,
  int originY)
{
  // keep active columns, to handle rowspan
  for (int k = 0; k < m_colCount; k++)
    m_colUsed[k] = 0;

  int y = 0;
  int r = 0;
  for (int i = 0; i < m_rows.length(); i++)
  {
    mgTableRow* row = (mgTableRow*) m_rows.getAt(i);

    int x = 0;
    int c = 0;
    for (int j = 0; j < row->m_columns.length(); j++)
    {
      mgTableCell* cell = (mgTableCell*) row->m_columns.getAt(j);

      // skip to next available cell
      while (m_colUsed[c] > 0)
        x += m_minWidths[c++];

      // figure width of cell spanning multiple columns
      int cellWidth = 0;
      for (int k = 0; k < cell->m_colSpan; k++)
      {
        cellWidth += m_minWidths[c];

        // set used for RowSpan rows
        m_colUsed[c] = cell->m_rowSpan;
        c++;
      }

      // figure height for cells spanning multiple rows
      int cellHeight = 0;
      for (int k = 0; k < cell->m_rowSpan; k++)
        cellHeight += m_minHeights[r+k];

      int fmtWidth = min(cellWidth, cell->m_maxWidth);
      int fmtHeight = min(cellHeight, cell->m_maxHeight);

      // figure vertical alignment
      int yAdjust = 0;
      switch (cell->m_vAlign)
      {
        case mgTableAlignTop:
          yAdjust = 0;
          break;
        case mgTableAlignBottom:
          yAdjust = cellHeight - fmtHeight;
          break;
        case mgTableAlignVCenter:
          yAdjust = (cellHeight - fmtHeight)/2;
          break;
        case mgTableAlignVFill:
          yAdjust = 0;
          fmtHeight = cellHeight;
          break;
      }

      // figure horizontal alignment
      int xAdjust = 0;
      switch (cell->m_hAlign)
      {
        case mgTableAlignLeft:
          xAdjust = 0;
          break;
        case mgTableAlignRight:
          xAdjust = cellWidth - fmtWidth;
          break;
        case mgTableAlignHCenter:
          xAdjust = (cellWidth - fmtWidth)/2;
          break;
        case mgTableAlignHFill:
          xAdjust = 0;
          fmtWidth = cellWidth;
          break;
      }

      // set the bounds of the box
      if (cell->m_child != NULL)
      {
        mgRectangle inside;
        inside.m_x = originX + x + xAdjust + cell->m_leftInset;
        inside.m_y = originY + y + yAdjust + cell->m_topInset;
        inside.m_width = fmtWidth - (cell->m_leftInset+cell->m_rightInset);
        inside.m_height = fmtHeight - (cell->m_topInset+cell->m_bottomInset);
        
        // adjust for cell bounds
        if (cell->m_frame != NULL)
          cell->m_frame->getInsideRect(inside);
          
        cell->m_child->setBounds(inside);
        mgDebug("position %08x at (%d, %d) (%d by %d)", cell->m_child, 
                inside.m_x, inside.m_y, inside.m_width, inside.m_height);
      }
      x += cellWidth;
    }
    // decrement all used cols
    for (int k = 0; k < c; k++)
      m_colUsed[k] = max(0, m_colUsed[k]-1);

    y += m_minHeights[r];

    r++;
  }
}

