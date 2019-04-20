#include "DS_Table.h"
#include "DS_OrderedList.h"
#include <string.h>
#include <assert.h>

using namespace DataStructures;

#ifdef _MSC_VER
#pragma warning( push )
#endif

#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void ExtendRows(Table::Row* input, int index)
{
	input->cells.Insert(new Table::Cell );
}

#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void FreeRow(Table::Row* input, int index)
{
	unsigned i;
	for (i=0; i < input->cells.Size(); i++)
	{
		delete input->cells[i];
	}
	delete input;
}
Table::Cell::Cell()
{
	isEmpty=true;
}
Table::Cell::~Cell()
{
	Clear();
}
Table::Cell& Table::Cell::operator = ( const Table::Cell& input )
{
	isEmpty=input.isEmpty;
	i=input.i;
	if (input.c)
	{
		if (c)
			delete [] c;
		c = new char [i];
		memcpy(c, input.c, i);
	}
	return *this;
}
Table::Cell::Cell( const Table::Cell & input)
{
	isEmpty=input.isEmpty;
	i=input.i;
	if (input.c)
	{
		if (c)
			delete [] c;
		c = new char [i];
		memcpy(c, input.c, i);
	}
}
void Table::Cell::Set(int input)
{
	assert(isEmpty);
	i=input;
	c=0;
	isEmpty=false;
}
void Table::Cell::Set(char *input)
{
	assert(isEmpty);
	if (input && input[0])
	{
		i=(int)strlen(input)+1;
		c = new char [i];
		strcpy(c, input);
	}
	else
		c=0;
	i=0;
	isEmpty=false;
}
void Table::Cell::Set(char *input, int inputLength)
{
	assert(isEmpty);
	if (input)
	{
		c = new char [inputLength];
		i=inputLength;
		memcpy(c, input, inputLength);
	}
	else
	{
		c=0;
		i=0;
	}
	isEmpty=false;
}
void Table::Cell::Get(int *output)
{
	assert(isEmpty==false);
	*output=i;
}
void Table::Cell::Get(char *output)
{
	assert(isEmpty==false);
	strcpy(output, c);
}
void Table::Cell::Get(char *output, int *outputLength)
{
	assert(isEmpty==false);
	memcpy(output, c, i);
	if (outputLength)
		*outputLength=i;
}
Table::Cell::Cell(int intValue, char *charValue, ColumnType type)
{
	if (type==NUMERIC)
	{
		Set(intValue);
	}
	else if (type==STRING)
	{
		Set(charValue);
	}
	else
	{
		Set(charValue, intValue);
	}

	isEmpty=false;
}
void Table::Cell::Clear(void)
{
	if (isEmpty==false)
		delete [] c;
	isEmpty=true;
}
Table::ColumnDescriptor::ColumnDescriptor()
{

}
Table::ColumnDescriptor::~ColumnDescriptor()
{

}
Table::ColumnDescriptor::ColumnDescriptor(char cn[_TABLE_MAX_COLUMN_NAME_LENGTH], ColumnType ct)
{
	columnType=ct;
	strcpy(columnName, cn);
}
void Table::Row::UpdateCell(unsigned columnIndex, int value)
{
	cells[columnIndex]->Clear();
	cells[columnIndex]->Set(value);

//	cells[columnIndex]->i=value;
//	cells[columnIndex]->c=0;
//	cells[columnIndex]->isEmpty=false;
}
void Table::Row::UpdateCell(unsigned columnIndex, char *str)
{
	cells[columnIndex]->Clear();
	cells[columnIndex]->Set(str);
}
void Table::Row::UpdateCell(unsigned columnIndex, int byteLength, char *data)
{
	cells[columnIndex]->Clear();
	cells[columnIndex]->Set(data,byteLength);
}
Table::Table()
{
}
Table::~Table()
{
	Clear();
}
unsigned Table::AddColumn(char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH], ColumnType columnType)
{
	if (columnName[0]==0)
		return (unsigned) -1;

	// Add this column.
	columns.Insert(Table::ColumnDescriptor(columnName, columnType));

	// Extend the rows by one
	rows.ForEachData(ExtendRows);

	return columns.Size()-1;
}
void Table::RemoveColumn(unsigned columnIndex)
{
	if (columnIndex >= columns.Size())
		return;

	columns.RemoveAtIndex(columnIndex);

	// Remove this index from each row.
	int i;
	DataStructures::Page<unsigned, Row*, _TABLE_BPLUS_TREE_ORDER> *cur = rows.GetListHead();
	while (cur)
	{
		for (i=0; i < cur->size; i++)
		{
			delete cur->data[i]->cells[columnIndex];
			cur->data[i]->cells.RemoveAtIndex(columnIndex);
		}

		cur=cur->next;
	}
}
unsigned Table::ColumnIndex(char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH])
{
	unsigned columnIndex;
	for (columnIndex=0; columnIndex<columns.Size(); columnIndex++)
		if (strcmp(columnName, columns[columnIndex].columnName)==0)
			return columnIndex;
	return (unsigned)-1;
}
char* Table::ColumnName(unsigned index)
{
	if (index >= columns.Size())
		return 0;
	else
		return (char*)columns[index].columnName;
}
Table::ColumnType Table::GetColumnType(unsigned index)
{
	if (index >= columns.Size())
		return (Table::ColumnType) 0;
	else
		return columns[index].columnType;
}
unsigned Table::GetColumnCount(void) const
{
	return columns.Size();
}
unsigned Table::GetRowCount(void) const
{
	return rows.Size();
}
Table::Row* Table::AddRow(unsigned rowId)
{
	Row *newRow;
	newRow = new Row;
	if (rows.Insert(rowId, newRow)==false)
	{
		delete newRow;
		return 0; // Already exists
	}
	unsigned rowIndex;
	for (rowIndex=0; rowIndex < columns.Size(); rowIndex++)
		newRow->cells.Insert( new Table::Cell() );
	return newRow;
}
Table::Row* Table::AddRow(unsigned rowId, DataStructures::List<Cell> &initialCellValues)
{
	Row *newRow = new Row;
	unsigned rowIndex;
	for (rowIndex=0; rowIndex < columns.Size(); rowIndex++)
	{
		if (rowIndex < initialCellValues.Size() && initialCellValues[rowIndex].isEmpty==false)
			newRow->cells.Insert(new Table::Cell(initialCellValues[rowIndex].i, initialCellValues[rowIndex].c, columns[rowIndex].columnType));
		else
			newRow->cells.Insert(new Table::Cell());
	}
	rows.Insert(rowId, newRow);
	return newRow;
}
Table::Row* Table::AddRowColumns(unsigned rowId, Row *row, DataStructures::List<unsigned> columnIndices)
{
	Row *newRow = new Row;
	unsigned columnIndex;
	for (columnIndex=0; columnIndex < columnIndices.Size(); columnIndex++)
	{
		if (row->cells[columnIndices[columnIndex]]->isEmpty==false)
		{
			newRow->cells.Insert(new Table::Cell(
				row->cells[columnIndices[columnIndex]]->i,
				row->cells[columnIndices[columnIndex]]->c,
				columns[columnIndex].columnType
				));
		}
		else
		{
			newRow->cells.Insert(new Cell());
		}
	}
	rows.Insert(rowId, newRow);
	return newRow;
}
void Table::RemoveRow(unsigned rowId)
{
	Row *out;
	if (rows.Delete(rowId, out))
		DeleteRow(out);
}
void Table::RemoveRows(Table *tableContainingRowIDs)
{
	unsigned i;
	DataStructures::Page<unsigned, Row*, _TABLE_BPLUS_TREE_ORDER> *cur = tableContainingRowIDs->GetRows().GetListHead();
	while (cur)
	{
		for (i=0; i < (unsigned)cur->size; i++)
		{
			rows.Delete(cur->keys[i]);
		}
		cur=cur->next;
	}
	return;
}
bool Table::UpdateCell(unsigned rowId, unsigned columnIndex, int value)
{
	assert(columns[columnIndex].columnType==NUMERIC);

	Row *row = GetRowByID(rowId);
	if (row)
	{
		row->UpdateCell(columnIndex, value);
		return true;
	}
	return false;
}
bool Table::UpdateCell(unsigned rowId, unsigned columnIndex, char *str)
{
	assert(columns[columnIndex].columnType==STRING);

	Row *row = GetRowByID(rowId);
	if (row)
	{
		row->UpdateCell(columnIndex, str);
		return true;
	}
	return false;
}
bool Table::UpdateCell(unsigned rowId, unsigned columnIndex, int byteLength, char *data)
{
	assert(columns[columnIndex].columnType==BINARY);

	Row *row = GetRowByID(rowId);
	if (row)
	{
		row->UpdateCell(columnIndex, byteLength, data);
		return true;
	}
	return false;
}
Table::FilterQuery::FilterQuery()
{

}
Table::FilterQuery::~FilterQuery()
{

}
Table::FilterQuery::FilterQuery(unsigned column, Cell *cell, FilterQueryType op)
{
	columnIndex=column;
	cellValue=cell;
	operation=op;
}
Table::Row* Table::GetRowByID(unsigned rowId)
{
	Row *row;
	if (rows.Get(rowId, row))
		return row;
	return 0;
}

Table::Row* Table::GetRowByIndex(unsigned rowIndex)
{
	DataStructures::Page<unsigned, Row*, _TABLE_BPLUS_TREE_ORDER> *cur = rows.GetListHead();
	while (cur)
	{
		if (rowIndex < (unsigned)cur->size)
			return cur->data[rowIndex];
		if (rowIndex <= (unsigned)cur->size)
			rowIndex-=cur->size;
		else
			return 0;
		cur=cur->next;
	}
	return 0;
}

void Table::QueryTable(unsigned *columnSubset, unsigned numColumnSubset, FilterQuery *inclusionFilters, unsigned numInclusionFilters, unsigned *rowIds, unsigned numRowIDs, Table *result)
{
	unsigned i;
	DataStructures::List<unsigned> columnIndicesToReturn;

	// Clear the result table.
	result->Clear();

	if (columnSubset && numColumnSubset>0)
	{
		for (i=0; i < numColumnSubset; i++)
		{
			if (columnSubset[i]>=0 && columnSubset[i]<columns.Size())
				columnIndicesToReturn.Insert(columnSubset[i]);
		}
	}
	else
	{
		for (i=0; i < columns.Size(); i++)
			columnIndicesToReturn.Insert(i);
	}

	if (columnIndicesToReturn.Size()==0)
		return; // No valid columns specified

	for (i=0; i < columnIndicesToReturn.Size(); i++)
	{
		result->AddColumn(columns[columnIndicesToReturn[i]].columnName,columns[columnIndicesToReturn[i]].columnType);
	}

	// Get the column indices of the filter queries.
	DataStructures::List<unsigned> inclusionFilterColumnIndices;
	if (inclusionFilters && numInclusionFilters>0)
	{
		for (i=0; i < numInclusionFilters; i++)
		{
			if (inclusionFilters[i].columnIndex>=0 && inclusionFilters[i].columnIndex<columns.Size())
				inclusionFilterColumnIndices.Insert(inclusionFilters[i].columnIndex);
			else
				inclusionFilterColumnIndices.Insert((unsigned)-1);
		}
	}

	if (rowIds==0 || numRowIDs==0)
	{
		// All rows
		DataStructures::Page<unsigned, Row*, _TABLE_BPLUS_TREE_ORDER> *cur = rows.GetListHead();
		while (cur)
		{
			for (i=0; i < (unsigned)cur->size; i++)
			{
				QueryRow(inclusionFilterColumnIndices, columnIndicesToReturn, cur->keys[i], cur->data[i], inclusionFilters, result);
			}
			cur=cur->next;
		}
	}
	else
	{
		// Specific rows
		Row *row;
		for (i=0; i < numRowIDs; i++)
		{
			if (rows.Get(rowIds[i], row))
			{
				QueryRow(inclusionFilterColumnIndices, columnIndicesToReturn, rowIds[i], row, inclusionFilters, result);
			}
		}
	}
}

void Table::QueryRow(DataStructures::List<unsigned> &inclusionFilterColumnIndices, DataStructures::List<unsigned> &columnIndicesToReturn, unsigned key, Table::Row* row, FilterQuery *inclusionFilters, Table *result)
{
	bool pass;
	unsigned columnIndex;
	unsigned j;

	// If no inclusion filters, just add the row
	if (inclusionFilterColumnIndices.Size()==0)
	{
		result->AddRowColumns(key, row, columnIndicesToReturn);
	}
	else
	{
		// Go through all inclusion filters.  Only add this row if all filters pass.
		for (j=0; j<inclusionFilterColumnIndices.Size(); j++)
		{
			columnIndex=inclusionFilterColumnIndices[j];
			if (row->cells[columnIndex]->isEmpty==false && columnIndex!=(unsigned)-1)
			{
				switch (inclusionFilters[j].operation)
				{
				case QF_EQUAL:
					switch(columns[inclusionFilterColumnIndices[j]].columnType)
					{
					case NUMERIC:
						pass=row->cells[columnIndex]->i==inclusionFilters[j].cellValue->i;
						break;
					case STRING:
						pass=strcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c)==0;
						break;
					case BINARY:
						pass=row->cells[columnIndex]->i==inclusionFilters[j].cellValue->i &&
							memcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c, row->cells[columnIndex]->i)==0;
						break;
					}
					break;
				case QF_NOT_EQUAL:
					switch(columns[inclusionFilterColumnIndices[j]].columnType)
					{
					case NUMERIC:
						pass=row->cells[columnIndex]->i!=inclusionFilters[j].cellValue->i;
						break;
					case STRING:
						pass=strcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c)!=0;
						break;
					case BINARY:
						pass=row->cells[columnIndex]->i==inclusionFilters[j].cellValue->i &&
							memcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c, row->cells[columnIndex]->i)==0;
						break;
					}
					break;
				case QF_GREATER_THAN:
					switch(columns[inclusionFilterColumnIndices[j]].columnType)
					{
					case NUMERIC:
						pass=row->cells[columnIndex]->i>inclusionFilters[j].cellValue->i;
						break;
					case STRING:
						pass=strcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c)>0;
						break;
					}
					break;
				case QF_LESS_THAN:
					switch(columns[inclusionFilterColumnIndices[j]].columnType)
					{
					case NUMERIC:
						pass=row->cells[columnIndex]->i<inclusionFilters[j].cellValue->i;
						break;
					case STRING:
						pass=strcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c)<0;
						break;
					}
					break;
				case QF_IS_EMPTY:
					pass=false;
					break;
				case QF_NOT_EMPTY:
					pass=true;
					break;
				}
			}
			else
			{
				if (inclusionFilters[j].operation==QF_IS_EMPTY)
					pass=true;
				else
					pass=false; // No value for this cell
			}

			if (pass==false)
				break;
		}

		if (pass)
		{
			result->AddRowColumns(key, row, columnIndicesToReturn);
		}
	}
}

static Table::SortQuery *_sortQueries;
static unsigned _numSortQueries;
static DataStructures::List<unsigned> *_columnIndices;
static DataStructures::List<Table::ColumnDescriptor> *_columns;
int RowSort(Table::Row* const &first, Table::Row* const &second) // first is the one inserting, second is the one already there.
{
	unsigned i, columnIndex;
	for (i=0; i<_numSortQueries; i++)
	{
		columnIndex=(*_columnIndices)[i];
		if (columnIndex==(unsigned)-1)
			continue;

		if (first->cells[columnIndex]->isEmpty==true && second->cells[columnIndex]->isEmpty==false)
			return 1; // Empty cells always go at the end

		if (first->cells[columnIndex]->isEmpty==false && second->cells[columnIndex]->isEmpty==true)
			return -1; // Empty cells always go at the end

		if (_sortQueries[i].operation==Table::QS_INCREASING_ORDER)
		{
			if ((*_columns)[columnIndex].columnType==Table::NUMERIC)
			{
				if (first->cells[columnIndex]->i>second->cells[columnIndex]->i)
					return 1;
				if (first->cells[columnIndex]->i<second->cells[columnIndex]->i)
					return -1;
			}
			else
			{
				// String
				if (strcmp(first->cells[columnIndex]->c,second->cells[columnIndex]->c)>0)
					return 1;
				if (strcmp(first->cells[columnIndex]->c,second->cells[columnIndex]->c)<0)
					return -1;
			}
		}
		else
		{
			if ((*_columns)[columnIndex].columnType==Table::NUMERIC)
			{
				if (first->cells[columnIndex]->i<second->cells[columnIndex]->i)
					return 1;
				if (first->cells[columnIndex]->i>second->cells[columnIndex]->i)
					return -1;
			}
			else
			{
				// String
				if (strcmp(first->cells[columnIndex]->c,second->cells[columnIndex]->c)<0)
					return 1;
				if (strcmp(first->cells[columnIndex]->c,second->cells[columnIndex]->c)>0)
					return -1;
			}
		}
	}

	return 0;
}
void Table::SortTable(Table::SortQuery *sortQueries, unsigned numSortQueries, Table::Row** out)
{
	unsigned i;
	unsigned outLength;
	DataStructures::List<unsigned> columnIndices;
	_sortQueries=sortQueries;
	_numSortQueries=numSortQueries;
	_columnIndices=&columnIndices;
	_columns=&columns;
	bool anyValid=false;

	for (i=0; i < numSortQueries; i++)
	{
		if (sortQueries[i].columnIndex>=0 && sortQueries[i].columnIndex<columns.Size() && columns[sortQueries[i].columnIndex].columnType!=BINARY)
		{
			columnIndices.Insert(sortQueries[i].columnIndex);
			anyValid=true;
		}
		else
			columnIndices.Insert((unsigned)-1); // Means don't check this column
	}

	DataStructures::Page<unsigned, Row*, _TABLE_BPLUS_TREE_ORDER> *cur;
	cur = rows.GetListHead();
	if (anyValid==false)
	{
		outLength=0;
		while (cur)
		{
			for (i=0; i < (unsigned)cur->size; i++)
			{
				out[(outLength)++]=cur->data[i];
			}
			cur=cur->next;
		}
		return;
	}

	// Start adding to ordered list.
	DataStructures::OrderedList<Row*, Row*, RowSort> orderedList;
	while (cur)
	{
		for (i=0; i < (unsigned)cur->size; i++)
		{
			orderedList.Insert(cur->data[i],cur->data[i]);
		}
		cur=cur->next;
	}

	outLength=0;
	for (i=0; i < orderedList.Size(); i++)
		out[(outLength)++]=orderedList[i];
}
void Table::PrintRow(char *out, int outLength, char columnDelineator, bool printDelineatorForBinary, Table::Row* inputRow)
{
	if (outLength<=0)
		return;
	if (outLength==1)
	{
		*out=0;
		return;
	}

	if (inputRow->cells.Size()!=columns.Size())
	{
		strncpy(out, "Cell width does not match column width.\n", outLength);
		out[outLength-1]=0;
		return;
	}

	char buff[512];
	unsigned i;
	int len;
	out[0]=0;
	for (i=0; i < columns.Size(); i++)
	{
        if (columns[i].columnType==NUMERIC)
		{
			if (inputRow->cells[i]->isEmpty==false)
			{
				sprintf(buff, "%i", inputRow->cells[i]->i);
				len=(int)strlen(buff);
			}
			else
				len=0;
			if (i+1!=columns.Size())
				buff[len++]=columnDelineator;
			buff[len]=0;
		}
		else if (columns[i].columnType==STRING)
		{
			if (inputRow->cells[i]->isEmpty==false && inputRow->cells[i]->c)
			{
				strncpy(buff, inputRow->cells[i]->c, 512-2);
				buff[512-2]=0;
				len=(int)strlen(buff);
			}
			else
				len=0;
			if (i+1!=columns.Size())
				buff[len++]=columnDelineator;
			buff[len]=0;
		}
		else
		{
			if (printDelineatorForBinary)
			{
				if (i+1!=columns.Size())
					buff[0]=columnDelineator;
				buff[1]=0;
			}
			else
				buff[0]=0;
			
		}

		len=(int)strlen(out);
		if (outLength==len+1)
			break;
		strncpy(out+len, buff, outLength-len);
		out[outLength-1]=0;
	}
}

void Table::Clear(void)
{
	rows.ForEachData(FreeRow);
	rows.Clear();
	columns.Clear(true);
}
List<Table::ColumnDescriptor>& Table::GetColumns(void)
{
	return columns;
}
DataStructures::BPlusTree<unsigned, Table::Row*, _TABLE_BPLUS_TREE_ORDER>& Table::GetRows(void)
{
	return rows;
}
DataStructures::Page<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER> * Table::GetListHead(void)
{
	return rows.GetListHead();
}
void Table::DeleteRow(Table::Row *row)
{
	unsigned rowIndex;
	for (rowIndex=0; rowIndex < row->cells.Size(); rowIndex++)
	{
		delete row->cells[rowIndex];
	}
	delete row;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
