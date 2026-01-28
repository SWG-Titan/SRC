# Visual Representation of HTML Metrics Output

## Screenshot Description

The generated HTML metrics page displays with the following visual layout:

### Header
```
═══════════════════════════════════════════════════════════
    Star Wars Galaxies - Server Metrics
═══════════════════════════════════════════════════════════
```
- Title in bright green (#4CAF50)
- Dark background (#1a1a1a)

### Summary Cards (Grid Layout)
Four prominent cards with dark backgrounds showing aggregate statistics:

```
┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│ Connected       │  │ Total Players   │  │ Total Objects   │  │ Total Servers   │
│ Servers         │  │                 │  │                 │  │                 │
│                 │  │                 │  │                 │  │                 │
│      5          │  │      247        │  │     45,234      │  │       5         │
│                 │  │                 │  │                 │  │                 │
└─────────────────┘  └─────────────────┘  └─────────────────┘  └─────────────────┘
```
- Gray labels above large green numbers
- Responsive grid layout
- Background color: #3a3a3a

### Server Details Table
Full-width table with alternating row hover effects:

```
┌──────────────┬──────────┬────────────┬──────────────┬─────────┬─────────┬───────────┬───────────┬─────────┐
│ Server Name  │ Scene    │ Process ID │ Status       │ Players │ Objects │ Creatures │ Buildings │ AI NPCs │
├──────────────┼──────────┼────────────┼──────────────┼─────────┼─────────┼───────────┼───────────┼─────────┤
│ GameServer   │ tatooine │ 12345      │ ● Connected  │ 42      │ 15,234  │ 3,421     │ 234       │ 1,523   │
│ GameServer   │ naboo    │ 12346      │ ● Connected  │ 38      │ 12,456  │ 2,845     │ 189       │ 1,234   │
│ GameServer   │ corellia │ 12347      │ ● Connected  │ 67      │ 17,544  │ 4,123     │ 287       │ 1,876   │
│ GameServer   │ dantooine│ 12348      │ ● Connected  │ 51      │ 13,821  │ 3,156     │ 198       │ 1,432   │
│ GameServer   │ yavin4   │ 12349      │ ● Connected  │ 49      │ 14,179  │ 3,267     │ 203       │ 1,489   │
└──────────────┴──────────┴────────────┴──────────────┴─────────┴─────────┴───────────┴───────────┴─────────┘
```
- Header row in green (#4CAF50)
- White text on green background
- Data rows in light gray text
- "Connected" status in green
- "Disconnected" status would be in red (#f44336)
- Hover effect: rows highlight in #3a3a3a

### Footer
```
                                                Last Updated: 2026-01-28 08:30:15
```
- Right-aligned timestamp
- Gray text (#aaa)
- Small font

## Color Scheme

**Background Colors:**
- Page background: #1a1a1a (very dark gray)
- Card backgrounds: #2d2d2d (dark gray)
- Item backgrounds: #3a3a3a (medium gray)

**Text Colors:**
- Primary text: #e0e0e0 (light gray)
- Labels: #aaa (medium gray)
- Accent (green): #4CAF50
- Error (red): #f44336

**Interactive:**
- Hover effect on table rows: #3a3a3a
- Borders: #444 (dark gray)

## Responsive Design

- Grid layout for summary cards
- Table scrolls horizontally on mobile
- Auto-adjusts to screen width
- Mobile-friendly touch targets

## Auto-Refresh

- Page refreshes every 60 seconds via meta tag
- Shows current timestamp of data
- No JavaScript required

## Example in Browser

When viewed in a web browser, the page appears as:
- Modern dark theme suitable for dashboards
- Clean, professional appearance
- Easy to read metrics at a glance
- Works in all modern browsers
- No special plugins required

The visual design emphasizes:
1. **Immediate visibility** - Key metrics in large numbers at top
2. **Status at a glance** - Color-coded connection status
3. **Detailed breakdown** - Complete table of all servers
4. **Professional appearance** - Modern UI design patterns
5. **Readability** - High contrast, clear typography
