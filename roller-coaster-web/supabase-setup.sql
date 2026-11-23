create table runs (
  id bigint generated always as identity primary key,
  created_at timestamptz default now(),
  type text,
  photo_url text,
  curve jsonb,
  status text default 'uploaded'
);

alter table runs enable row level security;
create policy "public read"   on runs for select using (true);
create policy "public insert" on runs for insert with check (true);
create policy "public update" on runs for update using (true);

alter table runs
  add column x_min double precision,
  add column x_max double precision;

grant select, insert, update on table public.runs to anon, authenticated;

alter publication supabase_realtime add table runs;

insert into storage.buckets (id, name, public) values ('photos', 'photos', true);
create policy "public upload photos" on storage.objects
  for insert with check (bucket_id = 'photos');
create policy "public read photos" on storage.objects
  for select using (bucket_id = 'photos');

