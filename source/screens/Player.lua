import 'CoreLibs/graphics'
import 'CoreLibs/timer'
import 'CoreLibs/object'

import './ScreenBase'
import '../screenManager.lua'
import '../gfxUtils.lua'
import '../utils.lua'

local gfx <const> = playdate.graphics

local PLAYDATE_W <const> = 400
local PLAYDATE_H <const> = 240

local ppm = PpmParser.new("./ppm/fdd.ppm")
local numFrames = ppm.numFrames

local layer = gfx.image.new(256, 192)

class('PlayerScreen').extends(ScreenBase)

function PlayerScreen:init()
  PlayerScreen.super.init(self)

  self.isPlaying = false

  self.prevFrame = -1
  self.currentFrame = 1

  self.prevFrameCrankAngle = 0
  self.currentCrankAngle = 0
  
  self.inputHandlers = {
    leftButtonDown = function()
      if self.currentFrame == 1 then
        self.currentFrame = numFrames
      else
        self.currentFrame = self.currentFrame - 1
      end
    end,
    rightButtonDown = function()
      if self.currentFrame == numFrames then
        self.currentFrame = 1
      else
        self.currentFrame = self.currentFrame + 1
      end
    end,
    downButtonDown = function()
      if self.isPlaying then
        self.isPlaying = false
      else
        self.isPlaying = true
      end
    end,
    BButtonUp = function()
      screenManager:setScreen('home')
    end,
    cranked = function(change, acceleratedChange)
      local newAngle = self.currentCrankAngle + change
      local diff = self.currentCrankAngle - self.prevFrameCrankAngle
      local frameDiff = math.floor(diff / 30)
      if (not self.isPlaying) and (not (frameDiff == 0)) then
        self:setCurrentFrame(self.currentFrame + frameDiff)
      end
      self.prevFrameCrankAngle += frameDiff * 30
      self.currentCrankAngle = newAngle
    end,
  }
end

function PlayerScreen:transitionEnter(t, id)
  -- initial entrance
  if id == nil then
    gfxUtils:drawBgGrid()
    self:update()
    gfxUtils:drawWhiteFade(t)
  -- inter-page transition
  elseif t >= 0.5 then
    gfxUtils:drawBgGrid()
    self:update()
    gfxUtils:drawWhiteFade((t - 0.5) * 2)
  end
end

function PlayerScreen:transitionLeave(t)
  if t < 0.5 then
    gfxUtils:drawBgGrid()
    self:update()
    gfxUtils:drawWhiteFade(1 - t * 2)
  end
end

function PlayerScreen:afterEnter()
  PlayerScreen.super.afterEnter(self)
  gfxUtils:drawBgGrid()
  self:update()
end

function PlayerScreen:setCurrentFrame(i)
  if i > 0 and i <= numFrames then
    self.currentFrame = i
  end
end

function PlayerScreen:update()
  gfx.setDrawOffset(0, 0)
  if not (self.currentFrame == self.prevFrame) then
    ppm:decodeFrameToBitmap(self.currentFrame, layer)
    self.prevFrame = self.currentFrame
  end
  -- draw frame 
  layer:draw(72, 16)
  -- local counterText = string.format("%03d/%03d", math.floor(i), 75);
  gfx.setColor(gfx.kColorWhite)
  gfx.fillRoundRect(PLAYDATE_W - 84, PLAYDATE_H - 26, 80, 22, 4)

  gfx.fillRoundRect((PLAYDATE_W / 2) - 80, PLAYDATE_H - 26, 160, 16, 4)

  -- TODO: proper frame tming
  if self.isPlaying then
    self:setCurrentFrame(self.currentFrame + 1)
    if self.currentFrame == numFrames then
      self.currentFrame = 1
    end
  end
end