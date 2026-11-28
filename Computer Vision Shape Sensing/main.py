import cv2
import numpy as np
import urllib.request
import json

from supabase import create_client, Client
import asyncio
from supabase import acreate_client, AsyncClient

url: str = "https://nwdfvgnkyamlatzhzlbw.supabase.co"
key: str = "sb_publishable_TG3krEs4FqGq7qXxlLrsHg_ispC5P-l"
 
async def genFunc(supabase, path, id):
    imageurl = "https://nwdfvgnkyamlatzhzlbw.supabase.co/storage/v1/object/public/photos/" + path
    req = urllib.request.urlopen(imageurl)
    arr = np.asarray(bytearray(req.read()), dtype=np.uint8)
    frame = cv2.imdecode(arr, cv2.IMREAD_COLOR)

    if frame is None:
        raise ValueError("Failed to decode image from URL.")

    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (9, 9))
    points = []

    # convert frame to HSV
    blurred = cv2.GaussianBlur(frame, (7, 7), 0)
    hsvFrame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # range of blue colors to detect
    lowerBlue = np.array([100, 100, 100])
    upperBlue = np.array([130, 255, 255])

    # lowerBlue = np.array([95, 30, 80])
    # upperBlue = np.array([125, 255, 255])

    # create a mask given the range of colors and apply it on the frame
    mask = cv2.inRange(hsvFrame, lowerBlue, upperBlue)

    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel, iterations=2)
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel, iterations=1)

    filtered = cv2.bitwise_and(frame, frame, mask=mask)

    # get the centers of the segments/blobs
    _, _, _, centroids = cv2.connectedComponentsWithStats(mask, 8)

    for center in centroids[1:]:
        point = tuple(np.round(center).astype(int))
        points.append(point)
        cv2.circle(filtered, point, 5, (0, 255, 0), -1)
        cv2.putText(filtered, f"({point[0]}, {point[1]})", (point[0] + 8, point[1]),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)

    print(points)

    # cv2.imshow("Filtered Image", filtered)

    # implementing function interpolation
    points = np.array(points)
    points = points[points[:, 0].argsort()]
    _, unique_idx = np.unique(points[:, 0], return_index=True)
    points = points[unique_idx]

    points = points - points[0]

    xi = points[:, 0]
    yi = -points[:, 1]

    coeffs = np.polyfit(xi, yi, deg=len(xi) - 1)
    poly_eq = np.poly1d(coeffs)
    print(coeffs.tolist())

    x_max = float(xi[-1])

    response = await (
        supabase.table("runs")
            .update({"curve": json.dumps(coeffs.tolist()),
                     "x_max": x_max, 
                     "status": "curveGenerated"})  
            .eq("id", id)                  
            .execute()
    )
    print(f"Row {id} updated:", response.data)


def handleInsert(payload: dict, supabase: AsyncClient):
    newRow = payload.get("data", {}).get("record", {})
    newPath = newRow.get("photo_url")
    if newPath:
        asyncio.create_task(genFunc(supabase, newPath, newRow.get("id")))

async def main():
    supabase: AsyncClient = await acreate_client(url, key)

    
    channel = supabase.channel("table-db-changes")
    await channel.on_postgres_changes(
        event="INSERT",
        schema="public",
        table="runs",  
        callback=lambda payload: handleInsert(payload, supabase)
    ).subscribe()

    while True:
        await asyncio.sleep(1)

if __name__ == "__main__":
    asyncio.run(main())


