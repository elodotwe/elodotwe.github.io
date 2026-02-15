class PhysicsObject {
    constructor(x, y, radius = 15, mass = 1, id = null) {
        this.x = x;
        this.initialX = x;
        this.y = y;
        this.initialY = y;
        this.radius = radius;
        this.mass = mass;
        this.vx = 0;
        this.vy = 0;
        this.id = id || Math.random().toString(36).substr(2, 9);
        this.forceType = 'attract'; // 'attract' or 'repel'
        this.isFixed = false;
        this.selected = false;
        this.color = this.randomizeColor();
        this.trail = [];
    }

    randomizeColor() {
        const hue = Math.random() * 360;
        return `hsl(${hue}, 70%, 50%)`;
    }

    applyForce(fx, fy) {
        if (this.isFixed) return;
        this.vx += fx / this.mass;
        this.vy += fy / this.mass;
    }

    update() {
        if (this.isFixed) return;
        this.trail.push({ x: this.x, y: this.y });
        this.x += this.vx;
        this.y += this.vy;
        this.vx *= 0.99;
        this.vy *= 0.99;
    }

    draw(ctx) {
        this.drawTrail(ctx);
        this.drawCircle(ctx);
    }

    drawTrail(ctx) {
        // Draw trail
        if (this.trail.length > 1) {
            ctx.strokeStyle = this.color;
            ctx.globalAlpha = 0.3;
            ctx.lineWidth = this.radius * 0.5;
            ctx.lineCap = 'round';
            ctx.beginPath();
            ctx.moveTo(this.trail[0].x, this.trail[0].y);
            for (let i = 1; i < this.trail.length; i++) {
                ctx.lineTo(this.trail[i].x, this.trail[i].y);
            }
            ctx.stroke();
            ctx.globalAlpha = 1;
        }
    }

    drawCircle(ctx) {
        ctx.beginPath();
        ctx.arc(this.x, this.y, this.radius, 0, Math.PI * 2);
        
        if (this.selected) {
            ctx.fillStyle = '#ffff00';
            ctx.fill();
            ctx.strokeStyle = '#0000ff';
            ctx.lineWidth = 3;
            ctx.stroke();
        } else {
            ctx.fillStyle = this.color;
            ctx.fill();
            ctx.strokeStyle = this.isFixed ? '#333' : '#222';
            ctx.lineWidth = 2;
            ctx.stroke();
        }

        // Draw fixed indicator
        if (this.isFixed) {
            ctx.fillStyle = 'rgba(0, 0, 0, 0.3)';
            ctx.fillRect(this.x - 3, this.y - 8, 6, 16);
        }
    }

    contains(x, y) {
        const dx = x - this.x;
        const dy = y - this.y;
        return dx * dx + dy * dy <= this.radius * this.radius;
    }
}

class PhysicsSim {
    constructor(canvasId) {
        this.canvas = document.getElementById(canvasId);
        this.ctx = this.canvas.getContext('2d');
        this.objects = [];
        this.selected = new Set();
        this.mode = 'edit';
        this.isRunning = false;
        this.isPaused = false;
        this.draggedObject = null;
        this.dragOffsetX = 0;
        this.dragOffsetY = 0;
        this.isTouchDragging = false;
        this.dragHistory = [];

        this.resizeCanvas();
        window.addEventListener('resize', () => this.resizeCanvas());

        this.setupEventListeners();
        this.animate();
    }

    resizeCanvas() {
        const rect = this.canvas.getBoundingClientRect();
        this.canvas.width = rect.width;
        this.canvas.height = rect.height;
    }

    setupEventListeners() {
        this.canvas.addEventListener('mousedown', (e) => this.onMouseDown(e));
        this.canvas.addEventListener('mousemove', (e) => this.onMouseMove(e));
        this.canvas.addEventListener('mouseup', (e) => this.onMouseUp(e));
        this.canvas.addEventListener('click', (e) => this.onClick(e));

        this.canvas.addEventListener('touchstart', (e) => this.onTouchStart(e));
        this.canvas.addEventListener('touchmove', (e) => this.onTouchMove(e));
        this.canvas.addEventListener('touchend', (e) => this.onTouchEnd(e));

        document.getElementById('editBtn').addEventListener('click', () => this.setMode('edit'));
        document.getElementById('runBtn').addEventListener('click', () => this.setMode('run'));
        document.getElementById('addObjectBtn').addEventListener('click', () => this.addObject());
        document.getElementById('removeSelectedBtn').addEventListener('click', () => this.removeSelected());
        document.getElementById('clearAllBtn').addEventListener('click', () => this.clearAll());
        document.getElementById('randomizeBtn').addEventListener('click', () => this.randomizeAll());
        document.getElementById('pauseBtn').addEventListener('click', () => this.togglePause());
    }

    setMode(mode) {
        if (mode === 'edit') {
            // Reset positions when leaving run mode
            this.objects.forEach(obj => {
                obj.x = obj.initialX;
                obj.y = obj.initialY;
                obj.vx = 0;
                obj.vy = 0;
                obj.trail = [];
            });
            this.isPaused = false;
        }
        
        this.mode = mode;
        this.isRunning = mode === 'run';
        this.draggedObject = null;

        document.getElementById('editBtn').classList.toggle('active', mode === 'edit');
        document.getElementById('runBtn').classList.toggle('active', mode === 'run');
        this.canvas.classList.toggle('edit-mode', mode === 'edit');
        
        const pauseBtn = document.getElementById('pauseBtn');
        if (pauseBtn) {
            pauseBtn.style.display = mode === 'run' ? 'block' : 'none';
            pauseBtn.classList.remove('paused');
        }
    }

    onMouseDown(e) {
        const rect = this.canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;

        for (let obj of this.objects) {
            if (obj.contains(x, y)) {
                this.draggedObject = obj;
                this.dragOffsetX = x - obj.x;
                this.dragOffsetY = y - obj.y;
                this.dragHistory = [{ x, y, time: Date.now() }];
                return;
            }
        }
    }

    onMouseMove(e) {
        if (!this.draggedObject) return;

        const rect = this.canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;

        this.draggedObject.x = x - this.dragOffsetX;
        this.draggedObject.y = y - this.dragOffsetY;

        this.dragHistory.push({ x, y, time: Date.now() });
        
        // Keep only last 500ms of history
        const now = Date.now();
        this.dragHistory = this.dragHistory.filter(p => now - p.time < 500);
    }

    onMouseUp() {
        if (this.draggedObject && this.isRunning && this.dragHistory.length > 1) {
            const oldest = this.dragHistory[0];
            const newest = this.dragHistory[this.dragHistory.length - 1];
            const timeDelta = Math.max(1, newest.time - oldest.time) / 1000; // Convert to seconds
            
            const deltaX = newest.x - oldest.x;
            const deltaY = newest.y - oldest.y;
            const velocityX = deltaX / timeDelta * 0.2;
            const velocityY = deltaY / timeDelta * 0.2;
            
            console.log('Fling released:', {
                historyLength: this.dragHistory.length,
                timeDeltaMs: newest.time - oldest.time,
                deltaX,
                deltaY,
                rawVelX: deltaX / timeDelta,
                rawVelY: deltaY / timeDelta,
                finalVelX: velocityX,
                finalVelY: velocityY
            });
            
            this.draggedObject.vx = velocityX;
            this.draggedObject.vy = velocityY;
        }
        this.draggedObject = null;
        this.dragHistory = [];
    }

    onTouchStart(e) {
        e.preventDefault();
        
        const touch = e.touches[0];
        const rect = this.canvas.getBoundingClientRect();
        const x = touch.clientX - rect.left;
        const y = touch.clientY - rect.top;

        let clicked = null;
        for (let obj of this.objects) {
            if (obj.contains(x, y)) {
                clicked = obj;
                this.draggedObject = obj;
                this.dragOffsetX = x - obj.x;
                this.dragOffsetY = y - obj.y;
                this.dragHistory = [{ x, y, time: Date.now() }];
                this.isTouchDragging = true;
                break;
            }
        }

        // Only handle selection in edit mode
        if (this.mode === 'edit' && clicked) {
            this.selected.forEach(id => {
                const obj = this.objects.find(o => o.id === id);
                if (obj) obj.selected = false;
            });
            this.selected.clear();
            this.selected.add(clicked.id);
            clicked.selected = true;
            this.updateProperties();
            this.updateStats();
        }
    }

    onTouchMove(e) {
        if (!this.draggedObject || !this.isTouchDragging) return;
        e.preventDefault();

        const touch = e.touches[0];
        const rect = this.canvas.getBoundingClientRect();
        const x = touch.clientX - rect.left;
        const y = touch.clientY - rect.top;

        this.draggedObject.x = x - this.dragOffsetX;
        this.draggedObject.y = y - this.dragOffsetY;

        this.dragHistory.push({ x, y, time: Date.now() });
        
        // Keep only last 500ms of history
        const now = Date.now();
        this.dragHistory = this.dragHistory.filter(p => now - p.time < 500);
    }

    onTouchEnd(e) {
        if (this.draggedObject && this.isRunning && this.dragHistory.length > 1) {
            const oldest = this.dragHistory[0];
            const newest = this.dragHistory[this.dragHistory.length - 1];
            const timeDelta = Math.max(1, newest.time - oldest.time) / 1000; // Convert to seconds
            
            const velocityX = (newest.x - oldest.x) / timeDelta * 0.2;
            const velocityY = (newest.y - oldest.y) / timeDelta * 0.2;
            
            this.draggedObject.vx = velocityX;
            this.draggedObject.vy = velocityY;
        }
        this.draggedObject = null;
        this.isTouchDragging = false;
        this.dragHistory = [];
    }

    onClick(e) {
        if (this.mode !== 'edit') return;

        const rect = this.canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;

        let clicked = null;
        for (let obj of this.objects) {
            if (obj.contains(x, y)) {
                clicked = obj;
                break;
            }
        }

        if (e.ctrlKey || e.metaKey) {
            if (clicked) {
                if (this.selected.has(clicked.id)) {
                    this.selected.delete(clicked.id);
                    clicked.selected = false;
                } else {
                    this.selected.add(clicked.id);
                    clicked.selected = true;
                }
            }
        } else {
            this.selected.forEach(id => {
                const obj = this.objects.find(o => o.id === id);
                if (obj) obj.selected = false;
            });
            this.selected.clear();

            if (clicked) {
                this.selected.add(clicked.id);
                clicked.selected = true;
            }
        }

        this.updateProperties();
        this.updateStats();
    }

    addObject() {
        const x = this.canvas.width / 2 + (Math.random() - 0.5) * 100;
        const y = this.canvas.height / 2 + (Math.random() - 0.5) * 100;
        const obj = new PhysicsObject(x, y);
        this.objects.push(obj);
        this.updateStats();
    }

    removeSelected() {
        this.objects = this.objects.filter(obj => !this.selected.has(obj.id));
        this.selected.clear();
        this.updateProperties();
        this.updateStats();
    }

    clearAll() {
        if (confirm('Clear all objects?')) {
            this.objects = [];
            this.selected.clear();
            this.updateProperties();
            this.updateStats();
        }
    }

    randomizeAll() {
        this.objects.forEach(obj => {
            obj.x = Math.random() * this.canvas.width;
            obj.y = Math.random() * this.canvas.height;
            obj.initialX = obj.x;
            obj.initialY = obj.y;
            obj.vx = 0;
            obj.vy = 0;
            obj.trail = [];
            obj.color = obj.randomizeColor();
        });
    }

    togglePause() {
        if (!this.isRunning) return;
        this.isPaused = !this.isPaused;
        const pauseBtn = document.getElementById('pauseBtn');
        pauseBtn.classList.toggle('paused', this.isPaused);
    }

    getSelectedObjects() {
        return this.objects.filter(obj => this.selected.has(obj.id));
    }

    updateProperties() {
        const content = document.getElementById('propertiesContent');
        const selected = this.getSelectedObjects();

        if (selected.length === 0) {
            content.innerHTML = '<p class="placeholder">Select object(s) to edit</p>';
            return;
        }

        const isSameValue = (getter) => {
            const values = selected.map(getter);
            return values.every(v => v === values[0]) ? values[0] : null;
        };

        const mass = isSameValue(obj => obj.mass);
        const forceType = isSameValue(obj => obj.forceType);
        const isFixed = isSameValue(obj => obj.isFixed);

        let html = '';
        if (selected.length === 1) {
            html += `<div class="property-group"><h4>Object ${selected[0].id.substr(0, 6)}</h4>`;
        } else {
            html += `<div class="property-group"><h4>${selected.length} Objects Selected</h4>`;
        }

        html += `
            <div class="property-item">
                <label class="property-label">Mass</label>
                <input type="number" class="property-input mass-input" min="0.1" step="0.1" value="${mass || ''}" placeholder="1.0">
            </div>
            <div class="property-item">
                <label class="property-label">Force Type</label>
                <select class="property-input force-type-input">
                    <option value="attract" ${forceType === 'attract' ? 'selected' : ''}>Attract</option>
                    <option value="repel" ${forceType === 'repel' ? 'selected' : ''}>Repel</option>
                </select>
            </div>
            <div class="property-item">
                <label class="checkbox-label">
                    <input type="checkbox" class="property-checkbox fixed-input" ${isFixed ? 'checked' : ''}>
                    Fixed Position
                </label>
            </div>
        </div>`;

        content.innerHTML = html;

        const massInput = content.querySelector('.mass-input');
        const forceTypeInput = content.querySelector('.force-type-input');
        const fixedInput = content.querySelector('.fixed-input');

        massInput.addEventListener('change', (e) => {
            const val = parseFloat(e.target.value);
            if (!isNaN(val) && val > 0) {
                selected.forEach(obj => obj.mass = val);
            }
        });

        forceTypeInput.addEventListener('change', (e) => {
            selected.forEach(obj => obj.forceType = e.target.value);
        });

        fixedInput.addEventListener('change', (e) => {
            selected.forEach(obj => obj.isFixed = e.target.checked);
        });
    }

    updateStats() {
        document.getElementById('objectCount').textContent = this.objects.length;
        document.getElementById('selectedCount').textContent = this.selected.size;
    }

    simulate() {
        const G = 0.5;

        for (let i = 0; i < this.objects.length; i++) {
            for (let j = i + 1; j < this.objects.length; j++) {
                const obj1 = this.objects[i];
                const obj2 = this.objects[j];

                const dx = obj2.x - obj1.x;
                const dy = obj2.y - obj1.y;
                const dist = Math.sqrt(dx * dx + dy * dy);

                if (dist < 10) continue;

                const force = (G * obj1.mass * obj2.mass) / (dist * dist);
                const fx = (force * dx) / dist;
                const fy = (force * dy) / dist;

                const obj1_repels = obj1.forceType === 'repel';
                const obj2_repels = obj2.forceType === 'repel';
                const forces_repel = obj1_repels || obj2_repels;

                if (forces_repel) {
                    obj1.applyForce(-fx, -fy);
                    obj2.applyForce(fx, fy);
                } else {
                    obj1.applyForce(fx, fy);
                    obj2.applyForce(-fx, -fy);
                }
            }
        }

        this.objects.forEach(obj => obj.update());
        this.checkBoundaries();
    }

    checkBoundaries() {
        this.objects.forEach(obj => {
            const bounce = 0.8; // Energy retention on bounce
            
            // Horizontal
            if (obj.x - obj.radius < 0) {
                obj.x = obj.radius;
                obj.vx *= -bounce;
            } else if (obj.x + obj.radius > this.canvas.width) {
                obj.x = this.canvas.width - obj.radius;
                obj.vx *= -bounce;
            }
            
            // Vertical
            if (obj.y - obj.radius < 0) {
                obj.y = obj.radius;
                obj.vy *= -bounce;
            } else if (obj.y + obj.radius > this.canvas.height) {
                obj.y = this.canvas.height - obj.radius;
                obj.vy *= -bounce;
            }
        });
    }

    draw() {
        this.ctx.fillStyle = '#2a2a2a';
        this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);

        // Draw all trails first
        this.objects.forEach(obj => obj.drawTrail(this.ctx));
        
        // Draw all objects on top
        this.objects.forEach(obj => obj.drawCircle(this.ctx));

        if (this.isRunning) {
            const statusText = this.isPaused ? 'PAUSED' : 'RUNNING';
            const statusColor = this.isPaused ? 'rgba(255, 165, 0, 0.2)' : 'rgba(0, 255, 0, 0.2)';
            this.ctx.fillStyle = statusColor;
            this.ctx.font = '16px monospace';
            this.ctx.fillText(statusText, 10, 30);
        }
    }

    animate() {
        if (this.isRunning && !this.isPaused) {
            this.simulate();
        }

        this.draw();
        requestAnimationFrame(() => this.animate());
    }
}

let sim;
document.addEventListener('DOMContentLoaded', () => {
    sim = new PhysicsSim('canvas');
});
