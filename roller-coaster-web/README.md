# IoT Web App

Website for choosing a physics type, uploading a track photo, and displaying the detected curve.
Backend: Supabase (database + photo storage + live updates).

## How the pipeline works
1. Website uploads the photo to the `photos` storage bucket and creates a row in `runs` with `status = "uploaded"`.
2. Pi finds rows where `status = "uploaded"`, downloads the photo, and sets `status = "processing"`.
3. Pi runs curve detection, writes the result to `curve`, and sets `status = "done"`.
4. Website sees the update live and draws the curve.

## `runs` table
| column | type | who sets it |
|---|---|---|
| id | number (auto) | database |
| created_at | timestamp (auto) | database |
| type | text: `"momentum"` or `"velocity"` | website |
| photo_url | text, file name in the `photos` bucket | website |
| curve | JSON list of [x, y] points | Pi |
| status | `"uploaded"` → `"processing"` → `"done"` | website, then Pi |

## Curve format
- `curve`: polynomial coefficients in pixel coordinates, highest power first
  (same order as numpy.polyfit). Example: [0.001, -1.2, 600]
- `x_min`, `x_max`: the pixel range where the strip starts and ends in the photo
- Pixel y increases downward; the website flips the y axis to match the photo.

## Supabase connection
- Project URL: https://vsnafdnhunahpuyvkgmw.supabase.co
- Publishable key: see `index.html` (safe to share)
- Never commit the secret key.

## Running the website locally
Open the folder in VS Code and use Live Server on `index.html`.



