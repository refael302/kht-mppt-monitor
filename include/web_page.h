#pragma once

static const char INDEX_HTML[] = R"MPPT(<!DOCTYPE html>
<html lang="he" dir="rtl">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta name="theme-color" content="#10211c">
<title>KHT MPPT</title>
<style>
  :root {
    --bg: #0c1411;
    --card: #16211c;
    --line: #2a3b33;
    --text: #f3f6f4;
    --muted: #9aada4;
    --amber: #f0b429;
    --green: #3dd68c;
    --red: #ff6b6b;
    --blue: #7eb6ff;
  }
  * { box-sizing: border-box; }
  body {
    margin: 0;
    font-family: "Segoe UI", Tahoma, sans-serif;
    background: radial-gradient(1200px 500px at 100% -10%, #1c3a2e, var(--bg));
    color: var(--text);
  }
  main { max-width: 520px; margin: 0 auto; padding: 20px 16px 32px; }
  header { display: flex; justify-content: space-between; align-items: center; gap: 12px; }
  h1 { font-size: 1.15rem; margin: 0; font-weight: 650; }
  .sub { color: var(--muted); font-size: 0.85rem; margin-top: 4px; }
  .pill { border-radius: 999px; padding: 6px 12px; font-size: 0.85rem; background: #24362e; white-space: nowrap; }
  .pill.ok { color: var(--green); }
  .pill.warn { color: var(--amber); }
  .pill.bad { color: var(--red); }
  .pill.done { color: var(--blue); }
  .hero, .tile {
    background: var(--card);
    border: 1px solid var(--line);
    border-radius: 18px;
  }
  .hero { margin-top: 18px; padding: 22px 18px; }
  .label { color: var(--muted); font-size: 0.9rem; }
  .hero .value, .tile .value {
    direction: ltr;
    unicode-bidi: isolate;
    text-align: right;
    white-space: nowrap;
  }
  .hero .value { font-size: 4rem; line-height: 1; margin: 8px 0 0; font-variant-numeric: tabular-nums; }
  .unit { color: var(--amber); font-size: 1.6rem; margin-left: 8px; }
  .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; margin-top: 12px; }
  .tile { border-radius: 16px; padding: 14px; }
  .tile .value { font-size: 1.7rem; margin-top: 6px; font-variant-numeric: tabular-nums; }
  .banner { display: none; margin-top: 12px; background: #3a1d1d; color: #ffc9c9; border-radius: 12px; padding: 12px; }
  .banner.show { display: block; }
  footer { margin-top: 16px; color: var(--muted); font-size: 0.78rem; line-height: 1.6; }
  #raw { font-family: ui-monospace, Consolas, monospace; direction: ltr; text-align: left; }
</style>
</head>
<body>
<main>
  <header>
    <div>
      <h1>KHT 200W MPPT</h1>
      <div class="sub">מוניטור טעינה</div>
    </div>
    <div id="status" class="pill">ממתין</div>
  </header>
  <div id="banner" class="banner">אין קליטה מהבקר. בדוק ש־T של הבקר מחובר ל־GPIO16, ויש אדמה משותפת.</div>
  <section class="hero">
    <div class="label">מתח טעינה</div>
    <div class="value"><span id="v">—</span><span class="unit">V</span></div>
  </section>
  <section class="grid">
    <article class="tile"><div class="label">זרם טעינה</div><div class="value"><span id="a">—</span> A</div></article>
    <article class="tile"><div class="label">הספק טעינה</div><div class="value"><span id="w">—</span> W</div></article>
    <article class="tile"><div class="label">טמפרטורה פנימית</div><div class="value"><span id="ti">—</span>°</div></article>
    <article class="tile"><div class="label">טמפרטורה חיצונית</div><div class="value"><span id="te">—</span>°</div></article>
  </section>
  <footer>
    <div id="meta">עדיין לא התקבלה מסגרת</div>
    <div id="raw"></div>
  </footer>
</main>
<script>
const $ = (id) => document.getElementById(id);
function fmt(n) { return Number(n).toFixed(1); }
async function tick() {
  try {
    const r = await fetch("/api", { cache: "no-store" });
    const d = await r.json();
    $("banner").classList.toggle("show", !d.link);
    $("v").textContent = d.link ? fmt(d.voltage_v) : "—";
    $("a").textContent = d.link ? fmt(d.current_a) : "—";
    $("w").textContent = d.link ? fmt(d.power_w) : "—";
    $("ti").textContent = d.link ? d.temp_internal_c : "—";
    $("te").textContent = (d.link && d.temp_external_c !== null) ? d.temp_external_c : "—";
    const pill = $("status");
    pill.textContent = d.link ? d.status_text : "אין קליטה";
    pill.className = "pill " + (d.link ? d.status_class : "bad");
    $("meta").textContent = "בתים " + d.bytes_seen + " · תקינות " + d.frames_ok + " · שגויות " + d.frames_bad +
      (d.link ? " · לפני " + (d.age_ms / 1000).toFixed(1) + " שנ׳" : "");
    $("raw").textContent = d.raw || "";
  } catch (e) {
    $("banner").classList.add("show");
    $("banner").textContent = "אין קשר לבקר. התחבר לרשת KHT-MPPT ופתח את 192.168.4.1";
  }
}
tick();
setInterval(tick, 500);
</script>
</body>
</html>)MPPT";
