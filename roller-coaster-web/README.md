# IoT Web App (Ishita)

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
A list of [x, y] points, e.g.
`[[0,0],[0.25,0.6],[0.5,0.2],[0.75,0.8],[1,0.1]]`

## Supabase connection
- Project URL: https://vsnafdnhunahpuyvkgmw.supabase.co
- Publishable key: see `index.html` (safe to share)
- Never commit the secret key.

## Running the website locally
Open the folder in VS Code and use Live Server on `index.html`.
