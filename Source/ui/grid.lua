grid = {}

-- scrolling grid pattern at different offsets
local GRID_PATTERNS <const> = {
  { 0x55, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF },
  { 0xFF, 0x55, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F },
  { 0x7F, 0xFF, 0x55, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF },
  { 0xFF, 0x7F, 0xFF, 0x55, 0xFF, 0x7F, 0xFF, 0x7F },
  { 0x7F, 0xFF, 0x7F, 0xFF, 0x55, 0xFF, 0x7F, 0xFF },
  { 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x55, 0xFF, 0x7F },
  { 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x55, 0xFF },
  { 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x55 },
}

function grid:draw(x, y, w, h)
  self:drawWithOffset(x, y, w, h, 0)
end

function grid:drawWithOffset(x, y, w, h, offset)
  x = x or 0
  y = y or 0
  w = w or PLAYDATE_W
  h = h or PLAYDATE_H
  gfx.setPattern(GRID_PATTERNS[math.floor(offset % 8) + 1])
  gfx.fillRect(x, y, w, h)
  -- white outline around the screen
  gfx.setColor(gfx.kColorWhite)
  gfx.drawRect(0, 0, PLAYDATE_W, PLAYDATE_H)
end