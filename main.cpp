#include <string>
#include <iostream>
#include <FileGDBAPI.h>
#include <timing.h>

using namespace std;
using namespace FileGDBAPI;

int main()
{
    fgdbError hr;
    wstring errorText;
    Geodatabase geodatabase;
    uint64 startTime;
    uint64 stopTime;

    // Benchmark FileGDB creation
    cout << "Benchmarking FileGDB Creation: ";
    startTime = GetTimeMs64();
    if ((hr = CreateGeodatabase(L"GdbPerfTest.gdb", geodatabase)) != S_OK)
    {
        wcout << "An error occured while creating the geodatabase." << endl;
        ErrorInfo::GetErrorDescription(hr, errorText);
        wcout << errorText << "(" << hr << ")." << endl;
        return -1;
    }
    stopTime = GetTimeMs64();
    cout << stopTime - startTime << endl;

    SpatialReference spatialReference;
    SpatialReferenceInfo srInfo;
    SpatialReferences::FindSpatialReferenceBySRID(3857, srInfo);
    spatialReference.SetSpatialReferenceText(srInfo.srtext);
    spatialReference.SetSpatialReferenceID(srInfo.auth_srid);

    // Benchmark feature class creation
    GeometryDef geometryDef;
    geometryDef.SetGeometryType(geometryPoint);
    geometryDef.SetSpatialReference(spatialReference);
    geometryDef.SetHasZ(false);
    geometryDef.SetHasM(false);

    std::vector<FieldDef> fieldDefs;
    FieldDef fieldDef1;
    fieldDef1.SetName(L"OBJECTID");
    fieldDef1.SetType(fieldTypeOID);
    fieldDef1.SetIsNullable(false);
    fieldDefs.push_back(fieldDef1);

    FieldDef fieldDef2;
    fieldDef2.SetName(L"Shape");
    fieldDef2.SetType(fieldTypeGeometry);
    fieldDef2.SetIsNullable(true);
    fieldDef2.SetGeometryDef(geometryDef);
    fieldDefs.push_back(fieldDef2);

    FieldDef fieldDef3;
    fieldDef3.SetName(L"NAMN");
    fieldDef3.SetType(fieldTypeString);
    fieldDef3.SetIsNullable(true);
    fieldDef3.SetLength(20);
    fieldDefs.push_back(fieldDef3);
    // Start timing
    startTime = GetTimeMs64();
    Table pointsTable;
    int recordCount = 0;
    if ((hr = geodatabase.CreateTable(L"\\points", fieldDefs, L"", pointsTable)) != S_OK)
    {
        wcout << "An error occurred while creating the table." << endl;
        ErrorInfo::GetErrorDescription(hr, errorText);
        wcout << errorText << "(" << hr << ")." << endl;

        // Report any extended errors (XML).
        ErrorInfo::GetErrorRecordCount(recordCount);
        for (int i = 0; i <= (recordCount - 1); i++)
        {
            ErrorInfo::GetErrorRecord(i, hr, errorText);
            wcout << errorText << endl;
        }
        ErrorInfo::ClearErrors();

        return -1;
    }
    stopTime = GetTimeMs64();
    wcout << "The points table was created in " << stopTime - startTime << " ms." << endl;

    // Benchmark creating points
    int numberOfPoints = 100000;
    Row newRow;
    pointsTable.CreateRowObject(newRow);
    newRow.SetString(L"NAMN", L"A point");
    startTime = GetTimeMs64();
    for (int i = 0; i < numberOfPoints; ++i)
    {
        // Create and assign a point geometry.
        PointShapeBuffer psb;
        hr = psb.Setup(shapePoint);

        Point *point;
        hr = psb.GetPoint(point);

        point->x = rand() % 361 + (-180);
        point->y = rand() % 181 + (-90);

        newRow.SetGeometry(psb);

        // Store the row.
        if ((hr = pointsTable.Insert(newRow)) != S_OK)
        {
            wcout << "An error occurred while inserting a row." << endl;
            ErrorInfo::GetErrorDescription(hr, errorText);
            wcout << errorText << "(" << hr << ")." << endl;
            return -1;
        }
    }
    stopTime = GetTimeMs64();
    wcout << "The " << numberOfPoints << " points were created in " << stopTime - startTime << " ms." << endl;
    // Close the table.
    if ((hr = geodatabase.CloseTable(pointsTable)) != S_OK)
    {
        wcout << "An error occurred while closing Streets." << endl;
        ErrorInfo::GetErrorDescription(hr, errorText);
        wcout << errorText << "(" << hr << ")." << endl;
        return -1;
    }

    // Close the geodatabase.
    if ((hr = CloseGeodatabase(geodatabase)) != S_OK)
    {
        wcout << "An error occurred while closing the geodatabase." << endl;
        ErrorInfo::GetErrorDescription(hr, errorText);
        wcout << errorText << "(" << hr << ")." << endl;
        return -1;
    }
    // Delete the geodatabase.
    if ((hr = DeleteGeodatabase(L"GdbPerfTest.gdb")) != S_OK)
    {
        wcout << "An error occurred while deleting the geodatabase." << endl;
        ErrorInfo::GetErrorDescription(hr, errorText);
        wcout << errorText << "(" << hr << ")." << endl;
        return -1;
    }
    wcout << "The geodatabase has been deleted." << endl;
    return 0;
}